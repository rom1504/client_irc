#include "FenPrincipale.h"

void FenPrincipale::envoyer()
{
	OngletServeur * serveur=ongletServeur(m_ongletCourant);
	if(m_ongletCourant->edition()->text()!="" && serveur->socket()->state()==QAbstractSocket::ConnectedState)
	{
		QString tempEntier=m_ongletCourant->edition()->text(),temp,commande,parametre;
		m_ongletCourant->edition()->setText("");
		for(int i=0;i<=tempEntier.count("\n");i++)
		{
			temp=tempEntier.section("\n",i,i);
			if(temp!="")
			{
				m_ongletCourant->edition()->ajouterTexte(temp);
				if(temp.startsWith("/"))
				{
					commande=temp.mid(1,temp.indexOf(" ")-1).toUpper();
					parametre=temp.section(" ",1);
					if(m_envoyerCommandes.contains(commande)) (this->*m_envoyerCommandes[commande])(parametre,m_ongletCourant);
					else serveur->envoyer(commande+" "+parametre);
				}
				else envoyerPrivmsg(temp,m_ongletCourant);
			}
		}
	}
}

void FenPrincipale::envoyerVersion(QString parametre,Onglet * onglet)
{
	ongletServeur(onglet)->envoyer("PRIVMSG "+parametre+" :VERSION");
}

void FenPrincipale::envoyerMsg(QString parametre,Onglet * onglet)
{
	if(!parametre.section(" ",0,0).startsWith("#") && parametre.section(" ",1)!="")
	{
		onglet->envoyer("PRIVMSG "+ parametre);
		envoyerMp(parametre.section(" ",0,0),onglet);
		changerOnglet(m_onglet.last());
		m_onglet.last()->ecrire("&lt;"+ongletServeur(onglet)->pseudo()+"&gt; "+parametre.section(" ",1),"gris");
	}
	else envoyerClientHelp("MSG",onglet);
}

void FenPrincipale::envoyerKickBan(QString parametre,Onglet * onglet)
{
	if(parametre!="" && onglet->type()==CHAN)
	{
		envoyerBan(parametre.section(" ",0,0),onglet);
		envoyerKick(parametre.section(" ",0,0)+" "+parametre.section(" ",1),onglet);
	}
}

void FenPrincipale::envoyerOp(QString parametre,Onglet * onglet)
{
	if(parametre!="" && onglet->type()==CHAN) envoyerMode(onglet->nom()+" +o "+parametre,onglet);
}

void FenPrincipale::envoyerDeOp(QString parametre,Onglet * onglet)
{
	if(parametre!="" && onglet->type()==CHAN) envoyerMode(onglet->nom()+" -o "+parametre,onglet);
}

void FenPrincipale::envoyerVoice(QString parametre,Onglet * onglet)
{
	if(parametre!="" && onglet->type()==CHAN) envoyerMode(onglet->nom()+" +v "+parametre,onglet);
}

void FenPrincipale::envoyerDeVoice(QString parametre,Onglet * onglet)
{
	if(parametre!="" && onglet->type()==CHAN) envoyerMode(onglet->nom()+" -v "+parametre,onglet);
}

void FenPrincipale::envoyerBan(QString parametre,Onglet * onglet)
{
	if(parametre!="" && onglet->type()==CHAN) envoyerMode(onglet->nom()+" +b "+parametre,onglet);
}

void FenPrincipale::envoyerDeBan(QString parametre,Onglet * onglet)
{
	if(parametre!="" && onglet->type()==CHAN) envoyerMode(onglet->nom()+" -b "+parametre,onglet);
}

void FenPrincipale::envoyerMode(QString parametre,Onglet * onglet)
{
	onglet->envoyer("MODE "+parametre);
}

void FenPrincipale::envoyerQuery(QString parametre,Onglet * onglet)
{
	if(!parametre.startsWith("#"))
	{
		envoyerMp(parametre,onglet);
		changerOnglet(m_onglet.last());
	}
}

void FenPrincipale::envoyerClose(QString parametre,Onglet * onglet)
{
	if(onglet->type()==SERVEUR) envoyerQuit(m_parametres["quitMessageParDefaut"],qobject_cast<OngletServeur *>(onglet));
	else if(onglet->type()==MP) quitterMp(qobject_cast<OngletMp *>(onglet));
	else envoyerPart(onglet->nom(),qobject_cast<OngletDiscussion *>(onglet)->serveur());
}

void FenPrincipale::envoyerWho(QString parametre,Onglet * onglet)
{
	OngletServeur * serveur=ongletServeur(onglet);
	QString chan=parametre;
	if(chan.at(0)=='#')
	{
		OngletChan * ongletChan=qobject_cast<OngletChan *>(discussion(chan,serveur));
		if(ongletChan!=NULL) ongletChan->augmenterNombreWhoDemande();
	}
	serveur->envoyer("WHO "+parametre);
}

void FenPrincipale::envoyerAway(QString parametre,Onglet * onglet)
{
	OngletServeur * serveur=ongletServeur(onglet);
	if(!serveur->absent())
	{
		QString raison=(parametre=="" ? m_parametres["awayMessageParDefaut"] : parametre);
		serveur->envoyer("AWAY "+raison);
		serveur->devenirAbsent();
		QList<Onglet *>::iterator i;
	    for(i=m_onglet.begin();i!=m_onglet.end();++i) if((*i)->type()==CHAN && qobject_cast<OngletChan *>(*i)->serveur()==serveur)
		{
			envoyerMe(tr("est Absent : ")+raison,(*i));
		}
	}
	else onglet->ecrire(tr("Vous etes déjà marqué comme absent."));
}

void FenPrincipale::envoyerBack(QString parametre,Onglet * onglet)
{
	OngletServeur * serveur=ongletServeur(onglet);
	if(serveur->absent())
	{
		serveur->envoyer("AWAY");
		serveur->devenirPresent();
		unsigned int t=serveur->tempsAbsence();
		unsigned int h=(int)(t/3600);
		unsigned int m=(int)((t-h*3600)/60);
		unsigned int s=t-h*3600-m*60;		
		QList<Onglet *>::iterator i;
	    for(i=m_onglet.begin();i!=m_onglet.end();++i) if((*i)->type()==CHAN && qobject_cast<OngletChan *>(*i)->serveur()==serveur)
		{
			envoyerMe(tr("est de retour ( parti ")+QString::number(h)+":"+QString::number(m)+":"+QString::number(s)+" ) ",(*i));
		}
	}
	else onglet->ecrire(tr("Vous etes déjà marqué comme présent."));
}

void FenPrincipale::envoyerInvite(QString parametre,Onglet * onglet)
{
	QString pseudo=parametre.section(" ",0,0),chan=parametre.section(" ",1,1);
	if(chan!="" || onglet->type()==CHAN) ongletServeur(onglet)->envoyer("INVITE "+pseudo+" "+(chan=="" ? onglet->nom() : chan));
}

void FenPrincipale::envoyerMKick(QString parametre,Onglet * onglet)
{
	if(onglet->type()==CHAN)
	{
		OngletChan * chan=qobject_cast<OngletChan *>(onglet);
		QList<QListWidgetItem *> connecte=chan->listeConnecte()->findItems("",Qt::MatchContains);
		for(QList<QListWidgetItem *>::iterator i=connecte.begin();i!=connecte.end();++i)
		{
			if((*i)->text()!=chan->serveur()->pseudo()) envoyerKick(chan->nom()+" "+(*i)->text(),chan);
		}
	}
}

void FenPrincipale::envoyerMOp(QString parametre,Onglet * onglet)
{
	if(onglet->type()==CHAN)
	{
		OngletChan * chan=qobject_cast<OngletChan *>(onglet);
		QList<QListWidgetItem *> connecte=chan->listeConnecte()->findItems("",Qt::MatchContains);
		for(QList<QListWidgetItem *>::iterator i=connecte.begin();i!=connecte.end();++i)
		{
			chan->envoyer("MODE "+chan->nom()+" +o "+((*i)->text()));
		}
	}
}

void FenPrincipale::envoyerReconnect(QString parametre,Onglet * onglet)
{
	if(parametre.toLower()=="all")
	{
		for(QList<Onglet *>::iterator i=m_onglet.begin();i!=m_onglet.end();++i)
		{
			if((*i)->type()==SERVEUR)
			{
				envoyerReconnect("",qobject_cast<OngletServeur *>(*i));
			}
		}
	}
	else
	{
		OngletServeur * serveur=ongletServeur(onglet);
		serveur->envoyer("QUIT :"+m_parametres["quitMessageParDefaut"]);
		serveur->reconnecter();
	}
}

void FenPrincipale::envoyerQuote(QString parametre,Onglet * onglet)
{
	ongletServeur(onglet)->envoyer(parametre);
}
void FenPrincipale::envoyerCharset(QString parametre,Onglet * onglet)
{
	QString nomCharset=parametre;
	Onglet * ongletOriginel=onglet;
	onglet=onglet->type()==MP ? qobject_cast<OngletDiscussion *>(onglet)->serveur() : onglet;
	QTextCodec * codec;
	if(nomCharset=="" || (codec=QTextCodec::codecForName(nomCharset.toUtf8()))==0) ongletOriginel->ecrire("Nom de charset invalide ou nul, charset utilisé actuelement : "+onglet->codec()->name());
	else
	{
		onglet->setCodec(codec);
		QFile * fichierCharset;
		QString temp="";
		fichierCharset=new QFile(m_emplacementDonnees+"/conf/charset.conf");
		fichierCharset->open(QIODevice::ReadOnly|QIODevice::Text);
		QMap<QString,QString> charset;
		QString identifieur=onglet->type()==SERVEUR ? ("@"+onglet->nom()) : (onglet->nom()+"@"+qobject_cast<OngletDiscussion *>(onglet)->serveur()->nom());
		bool mis=0;
		while(!fichierCharset->atEnd())
		{
			temp=fichierCharset->readLine();
			temp=temp.mid(0,temp.size()-1);
			if(temp.section(" ",0,0)!=identifieur) charset[temp.section(" ",0,0)]=temp.section(" ",1);
			else
			{
				charset[temp.section(" ",0,0)]=onglet->codec()->name();
				mis=1;
			}
		}
		fichierCharset->close();
		if(!mis) charset[identifieur]=onglet->codec()->name();
		fichierCharset->open(QIODevice::WriteOnly|QIODevice::Text);
		QTextStream streamCharset(fichierCharset);
		for(QMap<QString,QString>::iterator i=charset.begin();i!=charset.end();++i) streamCharset<<i.key()<<" "<<i.value()<<"\n";
		fichierCharset->close();
		ongletOriginel->ecrire("Charset bien modifié, charset utilisé actuelement : "+onglet->codec()->name());
	}
}
void FenPrincipale::envoyerCycle(QString parametre,Onglet * onglet)
{
	Onglet * ongletChoisi=parametre=="" && onglet->type()==CHAN ? onglet : discussion(parametre,onglet->type()==SERVEUR ? qobject_cast<OngletServeur *>(onglet) : qobject_cast<OngletDiscussion *>(onglet)->serveur());
	if(onglet!=NULL) ongletChoisi->envoyer("CYCLE "+(parametre=="" && onglet->type()==CHAN ? onglet->nom() : parametre));
}
void FenPrincipale::envoyerClientHelp(QString parametre,Onglet * onglet)
{
	if(parametre=="")
	{
		onglet->ecrire(tr("Commande supplémentaire ( non gérées par le serveur ) :"));
		QMap<QString,QString>::iterator i;
		QString texteCommandes="";
		for(i=m_aideCommandes.begin();i!=m_aideCommandes.end();++i)
		{
			texteCommandes+=i.key()+" ";
		}
		onglet->ecrire(texteCommandes);
		onglet->ecrire(tr("Taper /clienthelp &lt;commande&gt; pour plus d'information sur une commande en particulier ou /clienthelp -l pour plus d'informations sur toutes les commandes"));
	}
	else if(parametre=="-l")
	{
		onglet->ecrire(tr("Commande supplémentaire ( non gérées par le serveur ) :"));
		QMap<QString,QString>::iterator i;
		for(i=m_aideCommandes.begin();i!=m_aideCommandes.end();++i)
		{
			onglet->ecrire("[color="+(new QColor(199,50,50))->name()+"]"+i.key()+"[/color] : "+i.value(),"noir",i.key()+" : "+i.value());
		}
		onglet->ecrire(tr("Taper /clienthelp &lt;commande&gt; pour plus d'information sur une commande en particulier ou /clienthelp pour une liste des commandes"));
	}
	else if(m_aideCommandes.contains(parametre.toUpper()))
	{
		onglet->ecrire(tr("Utilisation : ")+m_aideCommandes[parametre.toUpper()]);
	}
	else
	{
		onglet->ecrire(tr("Cette commande n'est pas dans cette aide, peut être est-ce une commande serveur ?"));
	}
}
void FenPrincipale::envoyerKick(QString parametre,Onglet * onglet)
{
	QString chan=parametre.section(" ",0,0).startsWith("#") ? parametre.section(" ",0,0) : onglet->nom(),pseudo=parametre.section(" ",0,0).startsWith("#") ? parametre.section(" ",1,1) : parametre.section(" ",0,0),raison=parametre.section(" ",0,0).startsWith("#") ? parametre.section(" ",2) : parametre.section(" ",1);
	if(parametre.section(" ",0,0).startsWith("#")) onglet=discussion(chan,ongletServeur(onglet));
	if(onglet!=NULL) onglet->envoyer("KICK "+chan+" "+pseudo+(raison!="" ? (" :"+raison) : ""));
}

void FenPrincipale::envoyerSet(QString parametre,Onglet * onglet)
{
	if(parametre=="")
	{
		onglet->ecrire(tr("Parametres disponibles ( le nom un espace puis la valeur actuelle ) :"));
		for(QMap<QString,QString>::iterator i=m_parametres.begin();i!=m_parametres.end();++i)
		{
			onglet->ecrire(i.key()+" "+i.value());
		}
	}
	else
	{
		QString nom=parametre.section(" ",0,0),valeur=parametre.section(" ",1);
		if(m_parametres.contains(nom))
		{
			if(valeur=="")
			{
				onglet->ecrire(m_parametres.find(nom).key()+" "+m_parametres.find(nom).value());
			}
			else
			{
				m_parametres[nom]=valeur;
				QFile * fichierConf;
				fichierConf=new QFile(m_emplacementDonnees+"/conf/client_irc.conf");
				fichierConf->open(QIODevice::WriteOnly|QIODevice::Text);
				QTextStream streamConf(fichierConf);
				QMap<QString,QString>::iterator i;
				for(i=m_parametres.begin();i!=m_parametres.end();++i)
				{
					streamConf<<i.key()<<"="<<i.value()<<"\n";
				}
				fichierConf->close();
				onglet->ecrire(nom+tr(" mis à ")+valeur);
				mettreAJourOngletCss();
			}
		}
		else onglet->ecrire(tr("Ce paramètre n'existe pas"));
	}
}

void FenPrincipale::envoyerMe(QString parametre,Onglet * onglet)
{
	if(parametre!="")
	{
		if(onglet->type()==SERVEUR) onglet->ecrire(tr("Vous n'avez pas encore rejoint de discussion. Essayez /join #&lt;chan&gt; ou /msg &lt;pseudo&gt; &lt;message&gt;."),"noir");
		else
		{
			OngletServeur * serveur=qobject_cast<OngletDiscussion *>(onglet)->serveur();
			onglet->ecrire(serveur->pseudo()+" "+parametre);
			onglet->envoyer("PRIVMSG "+onglet->nom()+" :"+"ACTION "+parametre+"");
		}
	}
}

void FenPrincipale::envoyerPrivmsg(QString parametre,Onglet * onglet)
{
	if(parametre!="")
	{
		if(onglet->type()==SERVEUR) onglet->ecrire(tr("Vous n'avez pas encore rejoint de discussion. Essayez /join #&lt;chan&gt; ou /msg &lt;pseudo&gt; &lt;message&gt;."),"noir");
		else
		{
			int i,nbDepuisN=0,tailleCommande=QString("PRIVMSG "+onglet->nom()+" :").size();
			for(i=0;i<parametre.size();i++)
			{
				nbDepuisN++;
				if(nbDepuisN>400-tailleCommande)
				{
					parametre.insert(i,"\n");
					nbDepuisN=0;
				}
			}
			QString message;
			OngletServeur * serveur=qobject_cast<OngletDiscussion *>(onglet)->serveur();
			for(int i=0;i<=parametre.count("\n");i++)
			{
				message=parametre.section("\n",i,i);
				onglet->ecrire("&lt;"+serveur->pseudo()+"&gt; "+message,"gris");
				onglet->envoyer("PRIVMSG "+onglet->nom()+" :"+message);
				traitementMessagePlugin(serveur->pseudo(),message,onglet);
			}
		}
	}
}

void FenPrincipale::envoyerMp()
{
	QAction *sender=qobject_cast<QAction *>(QObject::sender());
	QList<Onglet *>::iterator i;
    for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->type()==CHAN && qobject_cast<OngletChan *>(*i)->actionOuvrirUnMp()==sender)
		{
			envoyerMp(qobject_cast<OngletChan *>(*i)->listeConnecte()->currentItem()->text(),qobject_cast<OngletDiscussion *>(*i)->serveur());
			break;
		}
	}
}
void FenPrincipale::envoyerMp(QString parametre,Onglet * onglet)
{
	QString nom=parametre;
	OngletServeur * serveur=ongletServeur(onglet);
	if(discussion(nom,serveur)==NULL)
	{
		OngletMp * nouveauChan=new OngletMp(nom,m_ongletCss,m_emplacementDonnees,serveur);
		m_onglet<<nouveauChan;
		ajouterElementDiscussionArbre(nouveauChan);
		changerOnglet(nouveauChan);
		connect(nouveauChan->edition(),SIGNAL(returnPressed()),this,SLOT(envoyer()));
		connect(nouveauChan->labelPseudo(),SIGNAL(clicked()),this,SLOT(envoyerNick()));
	}
}

void FenPrincipale::envoyerJoin(QString parametre,Onglet * onglet)
{
	onglet->envoyer("JOIN "+parametre);
}

void FenPrincipale::envoyerTopic()
{
	if(m_ongletCourant->type()!=SERVEUR)
	{
		QLineEdit *sender=qobject_cast<QLineEdit *>(QObject::sender());
		envoyerTopic(m_ongletCourant->nom()+" "+sender->text(),m_ongletCourant);
	}
}

void FenPrincipale::envoyerTopic(QString parametre,Onglet * onglet)
{
	QString sujet=parametre.section(" ",0,0).startsWith("#") ? parametre.section(" ",1) : parametre.section(" ",0),chan=parametre.section(" ",0,0).startsWith("#") ? parametre.section(" ",0,0) : onglet->nom();
	OngletServeur * serveur=onglet->type()==SERVEUR ? qobject_cast<OngletServeur *>(onglet) : qobject_cast<OngletDiscussion *>(onglet)->serveur();
	Onglet * ongletUtilise=discussion(chan,serveur);
	if(ongletUtilise!=NULL) ongletUtilise->envoyer("TOPIC "+chan+" :"+sujet);
}

void FenPrincipale::envoyerNick(QString parametre,Onglet * onglet)
{
	OngletServeur * serveur=onglet->type()==SERVEUR ? qobject_cast<OngletServeur *>(onglet) : qobject_cast<OngletDiscussion *>(onglet)->serveur();
	serveur->envoyer("NICK "+parametre);
	if(!serveur->pseudoAccepte())
	{
		serveur->setPseudo(parametre);
		serveur->labelPseudo()->setText(parametre);
	}
}

void FenPrincipale::envoyerPart(QString parametre,Onglet * onglet)
{
	QString nom=parametre.section(" ",0,0),raison=parametre.section(" ",1);
	Onglet * chan=nom!="" ? discussion(nom,onglet->type()==SERVEUR ? qobject_cast<OngletServeur *>(onglet) : qobject_cast<OngletDiscussion *>(onglet)->serveur()) : onglet;
	if(chan!=NULL && chan->type()==CHAN)
	{
		chan->envoyer("PART "+chan->nom()+(raison!="" ? (" :"+raison) : ""));
	}
}

void FenPrincipale::envoyerQuit()
{
	envoyerQuit("");
}

void FenPrincipale::envoyerQuit(QString parametre)
{
	envoyerQuit(parametre,m_ongletCourant);
}
void FenPrincipale::envoyerQuit(QString parametre,Onglet * onglet)
{
	OngletServeur * serveur=onglet->type()!=SERVEUR ? qobject_cast<OngletDiscussion *>(onglet)->serveur() : qobject_cast<OngletServeur *>(onglet);
	changerOnglet(NULL);
	QString raison=parametre=="" ? m_parametres["quitMessageParDefaut"] : parametre;
	serveur->envoyer("QUIT :"+raison);
	QList<Onglet *>::iterator i;
	for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->type()!=SERVEUR && qobject_cast<OngletDiscussion *>(*i)->serveur()==serveur)
		{
			serveur->elementArbre()->removeChild((*i)->elementArbre());
			m_onglet.removeOne(*i);
			--i;
		}
	}
	m_arbre->takeTopLevelItem(m_arbre->indexOfTopLevelItem(serveur->elementArbre()));
	disconnect(serveur->socket(),SIGNAL(readyRead()),this,SLOT(recevoir()));
	serveur->deconnecter();
	connect(serveur->socket(),SIGNAL(disconnected()),this,SLOT(supprimerLorsqueDeconnecte()));
	for(i=m_onglet.end()-1;i!=m_onglet.begin()-1;--i)
	{
		if(*i!=serveur)
		{
			changerOnglet(*i);
			break;
		}
	}
}

void FenPrincipale::envoyerNick()
{
	LabelPseudo *sender=qobject_cast<LabelPseudo *>(QObject::sender());
	QList<Onglet *>::iterator i;
	for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->labelPseudo()==sender)
		{
			bool ok;
			OngletServeur * serveur=(*i)->type()!=SERVEUR ? qobject_cast<OngletDiscussion *>(*i)->serveur() : qobject_cast<OngletServeur *>(*i);
			QString text=QInputDialog::getText(this,tr("Nouveau pseudo"),tr("Entrez le nouveau pseudo:"),QLineEdit::Normal,serveur->pseudo(),&ok);
			if(ok) envoyerNick(text,serveur);
			break;
		}
	}
}

void FenPrincipale::envoyerPing(QString parametre,Onglet * onglet)
{
	if(parametre.section(" ",0,0)=="") onglet->envoyer("PING "+QString::number(QDateTime::currentDateTime().toTime_t()));
	else onglet->envoyer("PRIVMSG "+parametre.section(" ",0,0)+" :\1PING "+QString::number(QDateTime::currentDateTime().toTime_t())+"\1");
}
