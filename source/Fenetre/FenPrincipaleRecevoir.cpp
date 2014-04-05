#include "FenPrincipale.h"

void FenPrincipale::recevoir()
{
	QTcpSocket * sender=qobject_cast<QTcpSocket *>(QObject::sender());
	OngletServeur * serveur;
	foreach(Onglet * onglet,m_onglet)
	{
		if(onglet->type()==SERVEUR && (serveur=qobject_cast<OngletServeur *>(onglet))->socket()==sender)
		{
			while(serveur->socket()->canReadLine()) serveur->ajouterAuBuffer(serveur->socket()->readLine());
			break;
		}
	}
}

void FenPrincipale::recevoir_()
{
	OngletServeur * serveur=qobject_cast<OngletServeur *>(QObject::sender());
	recevoir(serveur->prendreDuBuffer(),serveur);
}

void FenPrincipale::recevoir(QByteArray ligne,OngletServeur * serveur)
{
	QString recu=serveur->codec()->toUnicode(ligne);
	recu=recu.endsWith("\r\n") ? recu.mid(0,recu.size()-2) : recu;
	QString nomChan="";
	OngletChan * chan=NULL;
	QStringList parametres=recu.section(":",1,1).section(" ",1).split(" ");
	QStringList::iterator i;
	for(i=parametres.begin();i!=parametres.end();++i)
	{
		if((*i).startsWith("#"))
		{
			nomChan=*i;
			chan=qobject_cast<OngletChan *>(discussion(nomChan,serveur));
			if(chan!=NULL)
			{
				recu=chan->codec()->toUnicode(ligne);
				recu=recu.endsWith("\r\n") ? recu.mid(0,recu.size()-2) : recu;
			}
			break;
		}
	}
	QString commande=recu.section(" ",1,1);
	if(m_recevoirCommandes.contains(commande)) (this->*m_recevoirCommandes[commande])(recu,serveur);
	else if(recu.section(" ",0,0)=="PING") recevoirPing(recu,serveur);
	else if(commande=="366")
	{
		
	}
	else if(recu.startsWith("NOTICE AUTH")) serveur->ecrire((recu.section(" ",2).startsWith(":") ? recu.section(" ",2).mid(1) : recu.section(" ",2)));
	else
	{
		serveur->ecrire(recu);
	}
	m_ongletCourant->elementArbre()->setForeground(0,QBrush(QColor(0,0,0)));
}

void FenPrincipale::recevoirCommandeNumeriqueSansReaction(QString recu,OngletServeur * serveur)
{
	serveur->ecrire((recu.section(" ",3).startsWith(":") ? recu.section(" ",3).mid(1) : recu.section(" ",3)));
}

void FenPrincipale::recevoir002(QString recu,OngletServeur * serveur)
{
	QString temp=recu.section(" ",3).startsWith(":") ? recu.section(" ",3).mid(1) : recu.section(" ",3);
	serveur->setNom(temp.section(" ",3,3).mid(0,temp.section(" ",3,3).size()-1).section("[",0,0));
	mettreAJourTitreFenetre();
	serveur->ecrire(temp);
}

void FenPrincipale::recevoirInvite(QString recu,OngletServeur * serveur)
{
	QString inviteur=recu.section("!",0,0).mid(1);
	QString chan=recu.section(":",2,2);
	((m_ongletCourant->type()!=SERVEUR && qobject_cast<OngletDiscussion *>(m_ongletCourant)->serveur()==serveur) ? m_ongletCourant : serveur)->ecrire(tr("Vous avez été invité par ")+inviteur+tr(" sur ")+chan+"");
}

void FenPrincipale::recevoir317(QString recu,OngletServeur * serveur)//reception whois
{
	int bruteIdle=recu.section(" ",4,4).toInt();
	int sIdle,mIdle,hIdle;
	hIdle=int(bruteIdle/3600);
	mIdle=int(bruteIdle/60)-hIdle*60;
	sIdle=bruteIdle-(hIdle*3600+mIdle*60);
	QTime idle=QTime(hIdle,mIdle,sIdle);
	QDateTime signon=QDateTime::fromTime_t(recu.section(" ",5,5).toUInt());
	serveur->ecrire(recu.section(" ",3,3)+" "+tr("Inactif depuis : ")+idle.toString("hh:mm:ss")+tr(", Connecté le ")+signon.toString("d/M/yyyy")+tr(" à ")+signon.toString("hh:mm:ss"));
}
void FenPrincipale::recevoirNotice(QString recu,OngletServeur *serveur)
{
	if(recu.section(" ",2,2)=="AUTH") serveur->ecrire((recu.section(" ",3).startsWith(":") ? recu.section(" ",3).mid(1) : recu.section(" ",3)));
	else if(recu.mid(1).section(" ",0,0)==serveur->nom()) serveur->ecrire(recu.section(":",2));
	else if(recu.section(" ",0,0).section("@",1,1).section(".",-2,-1)==serveur->nom().section(".",-2,-1) && recu.section(" ",2,2)==serveur->pseudo())
	{
		QString message=recu.section(":",2);
		QString pseudo=recu.section(" ",0,0).section("@",0,0).mid(1).section("!",0,0);
		m_ongletCourant->ecrire("[color="+(new QColor(0,0,150))->name()+"]"+"-"+pseudo+"-"+"[/color] "+message,"noir","-"+pseudo+"-"+" "+message);
	}
	else recevoirPrivmsg(recu,serveur);
}
void FenPrincipale::recevoirTopic(QString recu,OngletServeur * serveur)
{
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(recu.section(" ",2,2),serveur));
	if(chan!=NULL)
	{
		QString sujet=recu.section(" ",3).mid(1);
		chan->sujet()->setText(sujet);
		chan->sujet()->setToolTip(tr("Le sujet de ")+chan->nom()+tr(" est : ")+sujet);
		chan->ecrire(recu.mid(1,recu.indexOf("!")-1)+tr(" a changé le sujet pour : ")+recu.section(" ",3).mid(1));
	}
}
void FenPrincipale::recevoir332(QString recu,OngletServeur * serveur)
{
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(recu.section(" ",3,3),serveur));
	if(chan!=NULL)
	{
		QString sujet=recu.section(" ",4).mid(1);
		chan->sujet()->setText(sujet);
		chan->sujet()->setToolTip(tr("Le sujet de ")+chan->nom()+tr(" est : ")+sujet);
		chan->ecrire(tr("Le sujet pour ")+recu.section(" ",3,3)+tr(" est : ")+recu.section(" ",4).mid(1),"topic");
	}
}
void FenPrincipale::recevoir333(QString recu,OngletServeur * serveur)
{
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(recu.section(" ",3,3),serveur));
	if(chan!=NULL)
	{
		QDateTime * date=new QDateTime;
		date->setTime_t(recu.section(" ",5,5).toUInt());
		chan->ecrire(tr("Le sujet pour ")+recu.section(" ",3,3)+tr(" a été modifié par ")+recu.section(" ",4,4)+tr(" à ")+date->toString("hh:mm:ss' le 'dd'/'MM'/'yyyy"),"topic");
	}
}
void FenPrincipale::recevoir001(QString recu,OngletServeur * serveur)
{
	serveur->ecrire(recu.section(":",2));
	serveur->elementArbre()->setText(0,recu.section(" ",6,6));
	if(!serveur->pseudoAccepte()) serveur->setPseudoAccepte(true);
	emit connecte(serveur);
}

void FenPrincipale::recevoir305(QString recu,OngletServeur * serveur)//Plus away
{
	recevoirCommandeNumeriqueSansReaction(recu,serveur);
	for(QList<Onglet *>::iterator i=m_onglet.begin();i!=m_onglet.end();++i) if((*i)->type()==CHAN && qobject_cast<OngletChan *>(*i)->serveur()==serveur)
	{
		qobject_cast<OngletChan *>(*i)->changerStatus(serveur->pseudo(),0);
	}
}

void FenPrincipale::recevoir306(QString recu,OngletServeur * serveur)//Marqué comme away
{
	recevoirCommandeNumeriqueSansReaction(recu,serveur);
	for(QList<Onglet *>::iterator i=m_onglet.begin();i!=m_onglet.end();++i) if((*i)->type()==CHAN && qobject_cast<OngletChan *>(*i)->serveur()==serveur)
	{
		qobject_cast<OngletChan *>(*i)->changerStatus(serveur->pseudo(),1);
	}
}

void FenPrincipale::recevoir315(QString recu,OngletServeur * serveur)//fin du who list
{
	OngletChan * ongletChan=qobject_cast<OngletChan *>(discussion(recu.section(" ",3,3),serveur));
	if(ongletChan!=NULL && ongletChan->whoDemande())
	{
		recevoirCommandeNumeriqueSansReaction(recu,serveur);
		ongletChan->baisserNombreWhoDemande();
	}
}

void FenPrincipale::recevoir352(QString recu,OngletServeur * serveur)
{
	OngletChan * ongletChan=qobject_cast<OngletChan *>(discussion(recu.section(" ",3,3),serveur));
	if(ongletChan!=NULL)
	{
		if(ongletChan->whoDemande()) recevoirCommandeNumeriqueSansReaction(recu,serveur);
		ongletChan->changerStatus(recu.section(" ",7,7),recu.section(" ",8,8).at(0)=='G');
	}
}

void FenPrincipale::recevoir353(QString recu,OngletServeur * serveur)
{
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(recu.section(" ",4,4),serveur));
	if(chan!=NULL)
	{
		int i=0;
		QString temp=recu.section(" ",5).mid(1);
		while(temp.section(" ",i,i)!="")
		{
			ajouterPersonne(chan,temp.section(" ",i,i));
			i++;
		}
	}
}

void FenPrincipale::recevoir474(QString recu,OngletServeur * serveur)
{
	QString nomChan=recu.section(" ",3,3);
	serveur->ecrire(tr("Vous ne pouvez pas rejoindre ")+nomChan+tr(" (Vous etes banni)"));
}

void FenPrincipale::recevoirMode(QString recu,OngletServeur * serveur)
{
	QString pseudo=recu.mid(1,recu.indexOf("!")-1);
	QString mode=recu.section(" ",3,3);
	QString pseudoMode=recu.section(" ",4,4);
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(recu.section(" ",2,2).startsWith(":") ? recu.section(" ",2,2).mid(1) : recu.section(" ",2,2),serveur));
	if(chan!=NULL) 
	{
		if(mode.mid(1)=="o" || mode.mid(1)=="v")
		{
			Connecte * item=dynamic_cast<Connecte *>(chan->listeConnecte()->findItems(pseudoMode,Qt::MatchExactly).first());
			QIcon *icone=NULL;
			if(mode.startsWith("+"))
			{
				if(mode.mid(1)=="o") icone=m_iconeOp;
				else if(mode.mid(1)=="v") icone=m_iconeVoice;
			}
			item->setIcon(mode.startsWith("-") ? QIcon() : *icone);
			item->setRang(mode.startsWith("-") ? "" : mode.mid(1));
			QString pseudoPersonne=(item->rang()=="o" ? "@" : (item->rang()=="v" ? "+" : ""))+item->text();
			enleverPersonne(chan,item->text());
			ajouterPersonne(chan,pseudoPersonne);
		}
		QString modeAction;
		if(mode.mid(1)=="o" || mode.mid(1)=="v" || mode.mid(1)=="b")
		{
			if(mode.startsWith("-")) modeAction=tr("retiré");
			else modeAction=tr("donné");
			if(mode.mid(1)=="o") modeAction+=tr(" le status d'operateur de chan");
			if(mode.mid(1)=="v") modeAction+=tr(" la parole");
			if(mode.mid(1)=="b") modeAction+=tr(" le ban");
		}
		else modeAction=mode;
		chan->ecrire(pseudo+tr(" a ")+modeAction+tr(" à ")+pseudoMode);
	}
}
void FenPrincipale::recevoirNick(QString recu,OngletServeur * serveur)
{
	QString pseudo=recu.mid(1,recu.indexOf("!")-1);
	QString nouveauPseudo=recu.section(" ",2,2).startsWith(":") ? recu.section(" ",2,2).mid(1) : recu.section(" ",2,2);
	QString tempAfficher=pseudo+tr(" s'appelle maintenant ")+nouveauPseudo;
	bool changementDeSonProprePseudo=false;
	if(pseudo==serveur->pseudo())
	{
		serveur->setPseudo(nouveauPseudo);
		serveur->labelPseudo()->setText(nouveauPseudo);
		if(!serveur->pseudoAccepte()) serveur->setPseudoAccepte(true);
		changementDeSonProprePseudo=true;
	}
	QList<Onglet *>::iterator i;
	QString rang;
	for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->type()!=SERVEUR && qobject_cast<OngletDiscussion *>(*i)->serveur()==serveur)
		{
			if((*i)->type()!=MP && qobject_cast<OngletChan *>(*i)->listeConnecte()->findItems(pseudo,Qt::MatchExactly).count()>0)
			{
				qobject_cast<OngletChan *>(*i)->listeConnecte()->findItems(pseudo,Qt::MatchExactly).first()->setText(nouveauPseudo);
				(*i)->ecrire(tempAfficher);
				rang=m_rang.mode(dynamic_cast<Connecte *>(qobject_cast<OngletChan *>(*i)->listeConnecte()->findItems(nouveauPseudo,Qt::MatchExactly).first())->rang()).rang;
				enleverPersonne(qobject_cast<OngletChan *>(*i),nouveauPseudo);
				ajouterPersonne(qobject_cast<OngletChan *>(*i),rang+nouveauPseudo);
			}
			if((*i)->type()==MP && (*i)->elementArbre()->text(0)==pseudo)
			{
				(*i)->elementArbre()->setText(0,nouveauPseudo);
				(*i)->setNom(nouveauPseudo);
			}
			if(changementDeSonProprePseudo) (*i)->labelPseudo()->setText(nouveauPseudo);
		}
	}
	if(changementDeSonProprePseudo) mettreAJourTitreFenetre();
}
void FenPrincipale::recevoirQuit(QString recu,OngletServeur * serveur)
{
	QString pseudo=recu.mid(1,recu.indexOf("!")-1);
	if(pseudo!=serveur->pseudo())
	{
		QString raison=recu.section(" ",2).mid(1);
		QList<Onglet *>::iterator i;
		for(i=m_onglet.begin();i!=m_onglet.end();++i)
		{
			if((*i)->type()!=SERVEUR && qobject_cast<OngletDiscussion *>(*i)->serveur()==serveur && (((*i)->type()==CHAN && !(qobject_cast<OngletChan *>(*i)->listeConnecte()->findItems(pseudo,Qt::MatchExactly).empty())) || ((*i)->type()==MP && pseudo==(*i)->nom())))
			{
				if((*i)->type()==CHAN) enleverPersonne(qobject_cast<OngletChan *>(*i),pseudo);
				(*i)->ecrire(pseudo+" ("+recu.section(" ",0,0).mid(1)+")"+tr(" a quitté ")+serveur->elementArbre()->text(0)+(raison!="" ? (" ("+raison+")") : ""),"quit");
			}
		}
	}
}
void FenPrincipale::recevoirPart(QString recu,OngletServeur * serveur)
{
	QString pseudo=recu.mid(1,recu.indexOf("!")-1);
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(recu.section(" ",2,2),serveur));
	if(chan!=NULL)
	{
		if(pseudo!=serveur->pseudo())
		{
			enleverPersonne(chan,pseudo);
			QString raison=recu.section(" ",3).mid(1);
			chan->ecrire(pseudo+" ("+recu.section(" ",0,0).mid(1)+")"+tr(" a quitté ")+recu.section(" ",2,2)+(raison!="" ? (" ("+raison+")") : "")+".","part");
		}
		else
		{
			if(m_ongletCourant==chan)
			{
				changerOnglet(serveur);
			}
			serveur->elementArbre()->removeChild(chan->elementArbre());
			m_onglet.removeOne(chan);
		}
	}
}
void FenPrincipale::recevoirKick(QString recu,OngletServeur * serveur)
{
	QString nomChan=recu.section(" ",2,2);
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(nomChan,serveur));
	if(chan!=NULL)
	{
			QString pseudo=recu.mid(1,recu.indexOf("!")-1),pseudoKicke=recu.section(" ",3,3),raison=recu.section(" ",4).mid(1);
			if(pseudoKicke==serveur->pseudo())
			{
				chan->ecrire(tr("Vous avez été kické de ")+nomChan+tr(" par ")+pseudo+" ("+raison+")");
				chan->connecter();
			}
			else
			{
				chan->ecrire(pseudo+tr(" a kické ")+pseudoKicke+tr(" de ")+nomChan+" ("+raison+")","kick");
				enleverPersonne(chan,pseudoKicke);
			}
	}
}
void FenPrincipale::recevoirJoin(QString recu,OngletServeur * serveur)
{	
	QString nomChan=recu.section(" ",2,2).startsWith(":") ? recu.section(" ",2,2).mid(1) : recu.section(" ",2,2);
	OngletChan * chan=qobject_cast<OngletChan *>(discussion(nomChan,serveur));
	if(nomChan.startsWith("#"))
	{
		QString pseudo=recu.mid(1,recu.indexOf("!")-1);
		if(pseudo==serveur->pseudo())
		{
			if(chan==NULL)
			{
				chan=new OngletChan(nomChan,m_ongletCss,m_emplacementDonnees,serveur);
				m_onglet<<chan;
				ajouterElementDiscussionArbre(chan);
				changerOnglet(chan);
				connect(chan->edition(),SIGNAL(returnPressed()),this,SLOT(envoyer()));
				connect(chan->labelPseudo(),SIGNAL(clicked()),this,SLOT(envoyerNick()));
				connect(chan->actionOuvrirUnMp(),SIGNAL(triggered()),this,SLOT(envoyerMp()));
				connect(chan->sujet(),SIGNAL(returnPressed()),this,SLOT(envoyerTopic()));
			}
			chan->ecrire(tr("Vous parlez maintenant sur ")+nomChan,"join");
		}
		else if(chan!=NULL)
		{
			pseudo=ajouterPersonne(chan,pseudo);
			chan->ecrire(pseudo+" ("+recu.section(" ",0,0).mid(1)+")"+tr(" a rejoint ")+nomChan,"join");
		}
	}
}

void FenPrincipale::recevoirPing(QString recu,OngletServeur * serveur)
{
	serveur->envoyer("PONG :"+recu.section(" :",1,1));
}

void FenPrincipale::recevoirPong(QString recu,OngletServeur * serveur)
{
	serveur->ecrire(tr("Réponse de ping de ")+recu.section(" ",2,2)+" : "+QString::number(QDateTime::fromTime_t(recu.section(":",2,2).toUInt()).secsTo(QDateTime::currentDateTime()))+tr(" seconde(s)"));
}

void FenPrincipale::recevoirPrivmsg(QString recu,OngletServeur * serveur)
{
	bool onRecoitUnMp;
	Onglet * chan=NULL;
	QString pseudo=recu.mid(1,recu.indexOf("!")-1);//pseudo de l'émetteur
	QString message=recu.section(" ",3).mid(1);
	QString destinataire=recu.section(" ",2,2).toLower();//clarifier le rôle de cette variable : destinataire de quoi ? + verif bien partout que la case est géré proprement
	if(destinataire==serveur->pseudo())// si le pseudo de l'utilisateur est le destinataire cela veut dire qu'on recoit un mp
	{
		destinataire=pseudo;
		onRecoitUnMp=1;
	}
	else onRecoitUnMp=0;
	if(recu.section(" ",1,1)=="NOTICE")
	{
		for(int j=0;j<=1 && chan==NULL;j++) // permet de commencer par vérifier si on peut mettre dans un mp puis dans un deuxième temps si on peut mettre dans un chan
		{
		    for(QList<Onglet *>::iterator i=m_onglet.begin();i!=m_onglet.end() && chan==NULL;++i) if((((*i)->type()==CHAN && j==1) || ((*i)->type()==MP && j==0)) && qobject_cast<OngletDiscussion *>(*i)->serveur()==serveur)
			{
				if(((*i)->type()==MP && (*i)->nom()==pseudo) || ((*i)->type()==CHAN && qobject_cast<OngletChan *>(*i)->listeConnecte()->findItems(pseudo,Qt::MatchExactly).count()==1))
				{
					chan=*i;
				}
			}
		}
		if(chan==NULL) chan=serveur;
	}
	else chan=discussion(destinataire,serveur);
	if(chan==NULL && onRecoitUnMp && !message.contains("\1VERSION\1") && (!message.contains("\1PING") || recu.section(" ",1,1)=="NOTICE"))
	{
		envoyerMp(destinataire,serveur);
		chan=discussion(destinataire,serveur);
		changerOnglet(chan);
	}
	if(message.contains("\1VERSION\1"))
	{
		serveur->envoyer("NOTICE "+pseudo+" :\1VERSION "+tr("client_irc 0.3 fait par rom1504")+"\1");
		serveur->ecrire(tr("Vous avez reçu un ctcp version de ")+pseudo);
	}
	else if(message.contains("\1PING") && recu.section(" ",1,1)=="PRIVMSG")
	{
		serveur->envoyer("NOTICE "+pseudo+" :"+message);
		serveur->ecrire(tr("Vous avez reçu un ctcp ping ")+message.replace(QRegExp("\1PING ([0-9]+)\1"),"\\1")+tr(" de ")+pseudo);
	}
	else if(chan!=NULL)
	{
		if(message.contains("\1ACTION")) chan->ecrire(pseudo+message.mid(7,message.size()-8));
		else if(message.contains("\1PING") && recu.section(" ",1,1)=="NOTICE") chan->ecrire(tr("Réponse de ping de ")+pseudo+" : "+QString::number(QDateTime::fromTime_t(message.replace(QRegExp("\1PING ([0-9]+)\1"),"\\1").toUInt()).secsTo(QDateTime::currentDateTime()))+tr(" seconde(s)"));
		else
		{
			if(message.contains("\1VERSION")) message=message.mid(1,message.size()-2);
			chan->ecrire("[color="+(message.contains(serveur->pseudo()) ? (new QColor(200,0,0)) : (new QColor(0,0,150)))->name()+"]"+(recu.section(" ",1,1)!="NOTICE" ? ("&lt;"+pseudo+"&gt;") : ("-"+pseudo+"-"))+"[/color] "+message,message.contains(serveur->pseudo()) ? "rouge" : "noir",(recu.section(" ",1,1)!="NOTICE" ? ("<"+pseudo+">") : ("-"+pseudo+"-"))+" "+message);
		}
		if(chan!=m_ongletCourant && recu.section(" ",1,1)!="NOTICE")
		{
			if(message.contains(serveur->pseudo())) chan->elementArbre()->setForeground(0,QBrush(QColor(0,0,255)));
			else chan->elementArbre()->setForeground(0,QBrush(QColor(255,0,0)));
		}
		if(!isActiveWindow() && m_parametres["notificationsDesactives"]!="1")
		{
			m_timerClignoter->start();
			if((message.contains(serveur->pseudo()) || chan->type()==MP) && recu.section(" ",1,1)!="NOTICE") QApplication::alert(this);
		}
		traitementMessagePlugin(pseudo,message,chan);
	}
}
