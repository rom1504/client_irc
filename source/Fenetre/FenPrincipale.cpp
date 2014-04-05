#include "FenPrincipale.h"

FenPrincipale::FenPrincipale() : QMainWindow()
{
	m_contructionTermine=false;
	
	//Emplacement donnees
	QStringList args=QCoreApplication::arguments();
	QString cheminFichierEmplacementDonnees=args.size()>1 ? args.at(1) : QDir::homePath()+"/.client_irc/";
	if(!QDir(cheminFichierEmplacementDonnees+"/conf").exists()) QDir(cheminFichierEmplacementDonnees).mkpath("conf");
	
	QFile * fichierEmplacementDonnees=new QFile(cheminFichierEmplacementDonnees+"/conf/emplacementDonnees.conf");
	if(!fichierEmplacementDonnees->exists())
	{
		m_emplacementDonnees=cheminFichierEmplacementDonnees;
		fichierEmplacementDonnees->open(QIODevice::WriteOnly | QIODevice::Text);
		fichierEmplacementDonnees->write(m_emplacementDonnees.toUtf8());
	}
	else
	{
		fichierEmplacementDonnees->open(QIODevice::ReadOnly);
		m_emplacementDonnees=fichierEmplacementDonnees->readAll();
	}
	fichierEmplacementDonnees->close();
	
	//création des dossiers
	if(!QDir(m_emplacementDonnees).exists()) QDir::root().mkpath(m_emplacementDonnees);
	QDir repertoireDonnees(m_emplacementDonnees);
	if(!QDir(m_emplacementDonnees+"/log").exists()) repertoireDonnees.mkdir("log");
	if(!QDir(m_emplacementDonnees+"/conf").exists()) repertoireDonnees.mkdir("conf");
	
	//creation et copie des fichiers au bon endroit
	QDir dossierConf(QCoreApplication::applicationDirPath());
	dossierConf.cdUp();
	dossierConf.cd("conf");
	foreach(QFileInfo fichier,dossierConf.entryInfoList(QDir::Files)) if(!QFile(m_emplacementDonnees+"/conf/"+fichier.fileName()).exists()) QFile(fichier.absoluteFilePath()).copy(m_emplacementDonnees+"/conf/"+fichier.fileName());
	
	//fichier de configuration
	QFile * fichierConf;
	QString temp;
	fichierConf=new QFile(m_emplacementDonnees+"/conf/client_irc.conf");
	fichierConf->open(QIODevice::ReadOnly|QIODevice::Text);
	while(!fichierConf->atEnd())
	{
		temp=fichierConf->readLine();
		temp=temp.mid(0,temp.size()-1);
		m_parametres[temp.section("=",0,0)]=temp.section("=",1,1);
	}
	fichierConf->close();
	if(m_parametres["cacherFenetrePrincipaleAuDemarrage"]=="1") hide();
	else showMaximized();
	
	//plugin
	QFile * fichierPlugin;
	fichierPlugin=new QFile(m_emplacementDonnees+"/conf/plugin.conf");
	fichierPlugin->open(QIODevice::ReadOnly|QIODevice::Text);
	while(!fichierPlugin->atEnd())
	{
		QString temp=fichierPlugin->readLine();
		temp=temp.mid(0,temp.size()-1);
		if(temp.startsWith("+"))
		{
			QPluginLoader loader(QCoreApplication::applicationDirPath()+"/plugin/"+temp.mid(1));
			QObject *plugin = loader.instance();
			if(plugin)
			{
				Plugin * plug = qobject_cast<Plugin *>(plugin);
				m_plugin<<plug;
				plug->initialiser();
				connect(plug,SIGNAL(messageReponse(QString,QString,QString)),this,SLOT(traitementMessageReponsePlugin(QString,QString,QString)));
			}
		}
	}
	fichierPlugin->close();
	
	//divers
	m_zoneCentrale=new QWidget;
	
	// layout principal
	m_layoutPrincipal=new QHBoxLayout;
	m_layoutPrincipal->setSpacing(0);
	
	//recevoir commandes
	m_recevoirCommandes["PRIVMSG"]=&FenPrincipale::recevoirPrivmsg;
	m_recevoirCommandes["NOTICE"]=&FenPrincipale::recevoirNotice;
	m_recevoirCommandes["JOIN"]=&FenPrincipale::recevoirJoin;
	m_recevoirCommandes["PART"]=&FenPrincipale::recevoirPart;
	m_recevoirCommandes["QUIT"]=&FenPrincipale::recevoirQuit;
	m_recevoirCommandes["NICK"]=&FenPrincipale::recevoirNick;
	m_recevoirCommandes["KICK"]=&FenPrincipale::recevoirKick;
	m_recevoirCommandes["MODE"]=&FenPrincipale::recevoirMode;
	m_recevoirCommandes["TOPIC"]=&FenPrincipale::recevoirTopic;
	m_recevoirCommandes["001"]=&FenPrincipale::recevoir001;
	m_recevoirCommandes["002"]=&FenPrincipale::recevoir002;
	m_recevoirCommandes["305"]=&FenPrincipale::recevoir305;// Plus away.
	m_recevoirCommandes["306"]=&FenPrincipale::recevoir306;// Marqué comme away.
	m_recevoirCommandes["315"]=&FenPrincipale::recevoir315;// Reçu après envoi d'un who. ( fin )
	m_recevoirCommandes["317"]=&FenPrincipale::recevoir317;
	m_recevoirCommandes["332"]=&FenPrincipale::recevoir332;
	m_recevoirCommandes["333"]=&FenPrincipale::recevoir333;
	m_recevoirCommandes["352"]=&FenPrincipale::recevoir352;// Reçu après envoi d'un who.
	m_recevoirCommandes["353"]=&FenPrincipale::recevoir353;
	m_recevoirCommandes["474"]=&FenPrincipale::recevoir474;
	m_recevoirCommandes["INVITE"]=&FenPrincipale::recevoirInvite;
	m_recevoirCommandes["PONG"]=&FenPrincipale::recevoirPong;//suite d'un /ping ( commande PING)
	
	m_commandesSansReaction<<"003"<<"004"<<"005";//001-099
	m_commandesSansReaction<<"200"<<"201"<<"202"<<"203"<<"204"<<"205"<<"206"<<"207"<<"208"<<"209"<<"210"<<"211"<<"212"<<"213"<<"214"<<"215"<<"216"<<"217"<<"218"<<"219"<<"221"<<"231"<<"232"<<"233"<<"234"<<"235"<<"240"<<"241"<<"242"<<"243"<<"244"<<"246"<<"247"<<"250"<<"251"<<"252"<<"253"<<"254"<<"255"<<"256"<<"257"<<"258"<<"259"<<"261"<<"262"<<"263"<<"265"<<"266";//200-299
	m_commandesSansReaction<<"300"<<"301"<<"302"<<"303"<<"311"<<"312"<<"313"<<"314"<<"316"<<"318"<<"319"<<"321"<<"322"<<"323"<<"324"<<"325"<<"331"<<"341"<<"342"<<"346"<<"347"<<"348"<<"349"<<"351"<<"361"<<"362"<<"363"<<"364"<<"365"<<"367"<<"368"<<"369"<<"371"<<"372"<<"373"<<"374"<<"375"<<"376"<<"381"<<"382"<<"383"<<"384"<<"391"<<"392"<<"393"<<"394"<<"395";//300-399
	m_commandesSansReaction<<"401"<<"402"<<"403"<<"404"<<"405"<<"406"<<"407"<<"408"<<"409"<<"411"<<"412"<<"413"<<"414"<<"415"<<"421"<<"422"<<"423"<<"424"<<"431"<<"432"<<"433"<<"436"<<"437"<<"441"<<"442"<<"443"<<"444"<<"445"<<"446"<<"451"<<"461"<<"462"<<"463"<<"464"<<"465"<<"466"<<"467"<<"471"<<"472"<<"473"<<"475"<<"476"<<"477"<<"478"<<"481"<<"482"<<"483"<<"484"<<"485"<<"491"<<"492"<<"501"<<"502";//400-599 reponse d'erreur
	for(QStringList::iterator i=m_commandesSansReaction.begin();i!=m_commandesSansReaction.end();++i) m_recevoirCommandes[*i]=&FenPrincipale::recevoirCommandeNumeriqueSansReaction;
	
	//envoyer commandes
	m_envoyerCommandes["JOIN"]=&FenPrincipale::envoyerJoin;
	m_envoyerCommandes["PART"]=&FenPrincipale::envoyerPart;
	m_envoyerCommandes["CYCLE"]=&FenPrincipale::envoyerCycle;
	m_envoyerCommandes["NICK"]=&FenPrincipale::envoyerNick;
	m_envoyerCommandes["SET"]=&FenPrincipale::envoyerSet;
	m_envoyerCommandes["CLIENTHELP"]=&FenPrincipale::envoyerClientHelp;
	m_envoyerCommandes["TOPIC"]=&FenPrincipale::envoyerTopic;
	m_envoyerCommandes["QUIT"]=&FenPrincipale::envoyerQuit;
	m_envoyerCommandes["ME"]=&FenPrincipale::envoyerMe;
	m_envoyerCommandes["KICK"]=&FenPrincipale::envoyerKick;
	m_envoyerCommandes["CHARSET"]=&FenPrincipale::envoyerCharset;
	m_envoyerCommandes["QUOTE"]=&FenPrincipale::envoyerQuote;
	m_envoyerCommandes["RECONNECT"]=&FenPrincipale::envoyerReconnect;
	m_envoyerCommandes["MOP"]=&FenPrincipale::envoyerMOp;
	m_envoyerCommandes["MKICK"]=&FenPrincipale::envoyerMKick;
	m_envoyerCommandes["INVITE"]=&FenPrincipale::envoyerInvite;
	m_envoyerCommandes["CLOSE"]=&FenPrincipale::envoyerClose;
	m_envoyerCommandes["QUERY"]=&FenPrincipale::envoyerQuery;
	m_envoyerCommandes["MODE"]=&FenPrincipale::envoyerMode;
	m_envoyerCommandes["OP"]=&FenPrincipale::envoyerOp;
	m_envoyerCommandes["DEOP"]=&FenPrincipale::envoyerDeOp;
	m_envoyerCommandes["VOICE"]=&FenPrincipale::envoyerVoice;
	m_envoyerCommandes["DEVOICE"]=&FenPrincipale::envoyerDeVoice;
	m_envoyerCommandes["BAN"]=&FenPrincipale::envoyerBan;
	m_envoyerCommandes["DEBAN"]=&FenPrincipale::envoyerDeBan;
	m_envoyerCommandes["KICKBAN"]=&FenPrincipale::envoyerKickBan;
	m_envoyerCommandes["MSG"]=&FenPrincipale::envoyerMsg;
	m_envoyerCommandes["VERSION"]=&FenPrincipale::envoyerVersion;
	m_envoyerCommandes["WHO"]=&FenPrincipale::envoyerWho;
	m_envoyerCommandes["AWAY"]=&FenPrincipale::envoyerAway;
	m_envoyerCommandes["BACK"]=&FenPrincipale::envoyerBack;
	m_envoyerCommandes["PING"]=&FenPrincipale::envoyerPing;
	m_envoyerCommandes["ACTION"]=&FenPrincipale::envoyerMe;
	
	//aide commandes
	m_aideCommandes["ME"]=tr("ME &lt;action&gt;,Envoi une action à l'onglet courant ( mp ou chan ).");
	m_aideCommandes["BAN"]=tr("BAN &lt;masque&gt;, Banni les utilisateurs correspondant au masque.");
	m_aideCommandes["DEBAN"]=tr("DEBAN &lt;masque&gt;, Debanni les utilisateurs correspondant au masque.");
	m_aideCommandes["OP"]=tr("OP &lt;pseudo&gt;, Donne le rang d'opérateur de chan.");
	m_aideCommandes["DEOP"]=tr("DEOP &lt;pseudo&gt;, Retire le rang d'opérateur de chan.");
	m_aideCommandes["VOICE"]=tr("VOICE &lt;pseudo&gt;, Donne la parole.");
	m_aideCommandes["DEVOICE"]=tr("DEVOICE &lt;pseudo&gt;, Retire la parole.");
	m_aideCommandes["MSG"]=tr("MSG &lt;pseudo&gt; &lt;message&gt;, Envoie un message privé.");
	m_aideCommandes["VERSION"]=tr("VERSION &lt;pseudo&gt;, Envoie une requête de version.");
	m_aideCommandes["SET"]=tr("SET [&lt;nom&gt;] [&lt;valeur&gt;], Modifie la valeur d'un parametre si le nom et la valeur sont founis, affiche la valeur du parametre si uniquement le nom est founi ou affiche le nom et la valeur de tous les parametres si aucun parametre n'est indiqué.");
	m_aideCommandes["CLIENTHELP"]=tr("CLIENTHELP [&lt;commande&gt;], Une aide sur les commandes non gérées par le serveur.");
	m_aideCommandes["KICKBAN"]=tr("KICKBAN &lt;pseudo&gt; [&lt;raison&gt;], Banni puis kick le pseudo du canal actuel ( une raison peut être donné pour le kick ).");
	m_aideCommandes["QUOTE"]=tr("QUOTE &lt;texte&gt;, Transmet le texte au serveur sous forme brute.");
	m_aideCommandes["CHARSET"]=tr("CHARSET &lt;charset&gt;, Change le charset utilisé dans l'onglet actuel ( sauf pour les message privé où le charset du serveur est changé ) si le charset fourni est valide et affiche le charset actuel.");
	m_aideCommandes["RECONNECT"]=tr("RECONNECT [-all], Reconnecte le serveur actuel ou tous les serveurs si -all est précisé.");
	m_aideCommandes["CLOSE"]=tr("CLOSE, ferme l'onglet actuel.");
	m_aideCommandes["QUERY"]=tr("QUERY &lt;pseudo&gt;, Ouvre un onglet de chat privé avec quelqu'un.");
	m_aideCommandes["MOP"]=tr("MOP, Donne le rand d'opérateur de chan à tous le monde dans le chan.");
	m_aideCommandes["MKICK"]=tr("MKICK, permet de kick tout le monde sauf vous du chan actuel.");
	m_aideCommandes["PING"]=tr("PING &lt;pseudo|canal&gt;, envoie un ping CTCP au pseudo ou au canal.");
	m_aideCommandes["ACTION"]=tr("ACTION &lt;action&gt;, équivalent de ME.");
	
	//champ et arbre
	m_layoutPrincipal->setAlignment(Qt::AlignLeft);
	m_arbre=new QTreeWidget;
	m_arbre->headerItem()->setHidden(true);
	m_ongletCourant=NULL;
	genererOngletCss();
	
	//icones
	m_iconeOwner=new QIcon(":images/owner.png");
	m_iconeBot=new QIcon(":images/bot.png");
	m_iconeOp=new QIcon(":images/op.png");
	m_iconeHalfOp=new QIcon(":images/halfop.png");
	m_iconeVoice=new QIcon(":images/voice.png");	
	m_iconeFenetre=new QIcon(":images/icone.png");
	setWindowIcon(*m_iconeFenetre);
	
	//rang
	m_rang.ajouter("~","q",m_iconeOwner,5);
	m_rang.ajouter("&","B",m_iconeBot,4);
	m_rang.ajouter("@","o",m_iconeOp,3);
	m_rang.ajouter("%","h",m_iconeHalfOp,2);
	m_rang.ajouter("+","v",m_iconeVoice,1);
	
	// ajouts des widgets dans les layout
	m_layoutPrincipal->addWidget(m_arbre,1);
	m_zoneCentrale->setLayout(m_layoutPrincipal);
    setCentralWidget(m_zoneCentrale);
	
	//actions
	m_actionFermer=new QAction(tr("&Fermer"),this);
	m_actionCacher=new QAction(tr("&Cacher"),this);
	m_actionQuitter=new QAction(tr("&Quitter"),this);
	m_actionConnecter=new QAction(tr("Se &connecter"),this);
	m_actionReconnecter=new QAction(tr("Se &reconnecter"),this);
	m_actionDeconnecter=new QAction(tr("Se &deconnecter"),this);
	m_actionPreference=new QAction(tr("&Préférence"),this);
	m_actionFavoris=new QAction(tr("&Favoris"),this);
	m_actionPlugin=new QAction(tr("Plu&gin"),this);
	m_actionAPropos=new QAction(tr("A &propos"),this);
	m_actionAide=new QAction(tr("&Aide"),this);
	
	//menus de la fenetre
	m_menuGeneral=menuBar()->addMenu(tr("&General"));
	m_menuGeneral->addAction(m_actionQuitter);
	m_menuServeur=menuBar()->addMenu(tr("&Serveur"));
	m_menuServeur->addAction(m_actionConnecter);
	m_menuServeur->addAction(m_actionReconnecter);
	m_menuServeur->addAction(m_actionDeconnecter);
	m_menuOutils=menuBar()->addMenu(tr("&Outils"));
	m_menuOutils->addAction(m_actionPreference);
	m_menuOutils->addAction(m_actionFavoris);
	m_menuOutils->addAction(m_actionPlugin);
	if(!m_plugin.isEmpty())
	{
		m_menuPlugin=m_menuOutils->addMenu(tr("Plugi&n"));
		foreach(Plugin * plugin,m_plugin) m_menuPlugin->addAction(plugin->action());
	}
	m_menuAide=menuBar()->addMenu(tr("&Aide"));
	m_menuAide->addAction(m_actionAide);
	m_menuAide->addAction(m_actionAPropos);
	
	//menu de m_arbre
	m_arbre->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_arbre->insertAction(0,m_actionFermer);
	
	//system tray
	m_iconeSysTray=new QIcon(":images/icone.png");
	m_iconeSysTrayNouveauMessage=new QIcon(":images/icone_nouveau_message.png");
	m_sysTray=new QSystemTrayIcon(*m_iconeSysTray);
	m_sysTray->show();
	m_timerClignoter=new QTimer;
	connect(m_timerClignoter,SIGNAL(timeout()),this,SLOT(clignoter()));
	m_timerClignoter->setInterval(500);
	m_menuSysTray=new QMenu(this);
	m_menuSysTray->addAction(m_actionCacher);
	m_menuSysTray->addSeparator();
	m_menuSysTray->addAction(m_actionQuitter);
	m_sysTray->setContextMenu(m_menuSysTray);
	m_sysTray->setToolTip("client_irc");
	
	//connexions
	connect(m_actionFermer,SIGNAL(triggered()),this,SLOT(fermer()));
	connect(m_actionConnecter,SIGNAL(triggered()),this,SLOT(connecterFenetre()));
	connect(m_actionReconnecter,SIGNAL(triggered()),this,SLOT(reconnecter()));
	connect(m_actionDeconnecter,SIGNAL(triggered()),this,SLOT(envoyerQuit()));
	connect(m_actionPreference,SIGNAL(triggered()),this,SLOT(ouvrirPreference()));
	connect(m_actionFavoris,SIGNAL(triggered()),this,SLOT(ouvrirFavoris()));
	connect(m_actionPlugin,SIGNAL(triggered()),this,SLOT(ouvrirPlugin()));
	connect(m_actionAPropos,SIGNAL(triggered()),this,SLOT(ouvrirAPropos()));
	connect(m_actionAide,SIGNAL(triggered()),this,SLOT(ouvrirAide()));
	connect(m_arbre,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(changementElement(QTreeWidgetItem*)));
	connect(m_sysTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(reduire(QSystemTrayIcon::ActivationReason)));
	connect(m_actionQuitter,SIGNAL(triggered()),this,SLOT(quitter()));
	connect(m_actionCacher,SIGNAL(triggered()),this,SLOT(cacher()));
	
	//bool
	m_clignotant=false;
	m_contructionTermine=true;
	m_ongletOuvert=false;
	m_fermer=false;
	
	//fenetres auxiliaires
	m_fenConnexion=NULL;
	m_fenPreference=NULL;
	m_fenAPropos=NULL;
	m_fenFavoris=NULL;
	m_fenPlugin=NULL;
	
	//autojoin
	if(m_parametres["autoJoin"].toInt()==1) autoJoin();
}

void FenPrincipale::traitementMessagePlugin(QString pseudo,QString message,Onglet * onglet)
{
	foreach(Plugin * plugin,m_plugin)
	{
		QString reponse=plugin->reponse(message);
		if(reponse!="") envoyerPrivmsg(reponse,onglet);
		if((qobject_cast<OngletDiscussion *>(onglet)->serveur()->pseudo())!=pseudo) plugin->messageRecu(pseudo,onglet->nom(),qobject_cast<OngletDiscussion *>(onglet)->serveur()->nom(),message);
	}
}

void FenPrincipale::traitementMessageReponsePlugin(QString emetteur,QString serveur,QString message)
{
	envoyerPrivmsg(message,discussion(emetteur,trouverServeur(serveur)));
}

OngletServeur * FenPrincipale::ongletServeur(Onglet * onglet)
{
	return onglet->type()==SERVEUR ? qobject_cast<OngletServeur *>(onglet) : qobject_cast<OngletDiscussion *>(onglet)->serveur();
}

void FenPrincipale::mettreAJourOngletCss()
{
	genererOngletCss();
	for(QList<Onglet *>::iterator i=m_onglet.begin();i!=m_onglet.end();++i) (*i)->setCss(m_ongletCss);
}

void FenPrincipale::genererOngletCss()
{
	m_ongletCss="";
	for(QMap<QString,QString>::iterator i=m_parametres.begin();i!=m_parametres.end();++i) if((i.key()).startsWith("couleur")) m_ongletCss+="."+i.key().mid(7).toLower()+",."+i.key().mid(7).toLower()+" a"+"{color:"+*i+";}";
}

void FenPrincipale::reconnecter()
{
	envoyerReconnect("",m_ongletCourant->type()==SERVEUR ? qobject_cast<OngletServeur *>(m_ongletCourant) : (qobject_cast<OngletDiscussion *>(m_ongletCourant))->serveur());
}

void FenPrincipale::fermer()
{
	envoyerClose("",m_ongletCourant);
}

void FenPrincipale::cacher()
{
	if(isVisible())
	{
		m_actionCacher->setText(tr("Afficher"));
		hide();
	}
	else
	{
		m_actionCacher->setText(tr("Cacher"));
		showMaximized();
	}
}

void FenPrincipale::connecterFenetre()
{
	m_fenConnexion=new FenConnexion(m_parametres);
	connect(m_fenConnexion,SIGNAL(connecter(QString,QString,int)),this,SLOT(connecter_(QString,QString,int)));
}

void FenPrincipale::ouvrirPreference()
{
	m_fenPreference=new FenPreference(&m_parametres,m_emplacementDonnees);
	connect(m_fenPreference,SIGNAL(parametreChange()),this,SLOT(mettreAJourOngletCss()));
}

void FenPrincipale::ouvrirFavoris()
{
	m_fenFavoris=new FenFavoris(m_emplacementDonnees);
	connect(m_fenFavoris,SIGNAL(connection(QString)),this,SLOT(connecter(QString)));
	connect(m_fenFavoris,SIGNAL(join(QString,QString)),this,SLOT(rejoindre(QString,QString)));
}

void FenPrincipale::ouvrirPlugin()
{
	m_fenPlugin=new FenPlugin(m_emplacementDonnees);
}

void FenPrincipale::rejoindre(QString discussion,QString serveur)
{
	OngletServeur * ongletServeur=trouverServeur(serveur);
	if(ongletServeur!=NULL)
	{
		if(discussion.startsWith("#")) envoyerJoin(discussion,ongletServeur);
		else envoyerMp(discussion,ongletServeur);
	}
}

void FenPrincipale::ouvrirAPropos()
{
	m_fenAPropos=new FenAPropos;
}

void FenPrincipale::ouvrirAide()
{
	QMessageBox * aide=new QMessageBox (QMessageBox::Information,tr("Aide"),tr("Pour avoir de l'aide sur les commandes spécifiques au client utilisez la commande /clienthelp"),QMessageBox::NoButton,this);
	aide->show();
}

void FenPrincipale::quitter()
{
	m_fermer=1;
	close();
}

void FenPrincipale::closeEvent(QCloseEvent *e)
{
	if(m_fermer || m_parametres["nePasQuitter"]=="0")
	{
		delete m_sysTray;
		if(m_fenConnexion!=NULL) delete m_fenConnexion;
		if(m_fenPreference!=NULL) delete m_fenPreference;
		if(m_fenAPropos!=NULL) delete m_fenAPropos;
		if(m_fenFavoris!=NULL) delete m_fenFavoris;
		if(m_fenPlugin!=NULL) delete m_fenPlugin;
		for(QList<Onglet *>::iterator i=m_onglet.begin();i!=m_onglet.end();++i) if((*i)->type()==SERVEUR) envoyerQuit(m_parametres["quitMessageParDefaut"],qobject_cast<OngletServeur *>(*i));
		foreach(Plugin * plugin,m_plugin) delete plugin;
		e->accept();
	}
	else
	{
		cacher();
		e->ignore();
	}
}

bool FenPrincipale::event(QEvent * e)
{
	QWidget::event(e);
	if(m_contructionTermine && e->type()==QEvent::WindowActivate && m_timerClignoter->isActive())
	{
		m_timerClignoter->stop();
		m_sysTray->setIcon(*m_iconeSysTray);
	}
	return true;
}

void FenPrincipale::autoJoin()
{
	QFile * fichierAutoJoin;
	QString temp;
	fichierAutoJoin=new QFile(m_emplacementDonnees+"/conf/favoris.conf");
	fichierAutoJoin->open(QIODevice::ReadOnly|QIODevice::Text);
	while(!fichierAutoJoin->atEnd())
	{
		temp=fichierAutoJoin->readLine();
		if(temp.startsWith("+@")) connecter(temp.mid(2,temp.size()-3),m_parametres["pseudoParDefaut"],m_parametres["portParDefaut"].toInt(),true);
	}
	connect(this,SIGNAL(connecte(OngletServeur *)),this,SLOT(suiteAutoJoin(OngletServeur *)));
	fichierAutoJoin->close();
}
void FenPrincipale::suiteAutoJoin(OngletServeur * serveur)
{
	rejoindreLesChansOuverts(serveur);
	if(serveur->isOuvertAvecAutoJoin() || m_parametres["superAutoJoin"].toInt()==1)
	{
		QFile * fichierAutoJoin;
		QString temp;
		fichierAutoJoin=new QFile(m_emplacementDonnees+"/conf/favoris.conf");
		fichierAutoJoin->open(QIODevice::ReadOnly|QIODevice::Text);
		temp="";
		bool go=0;
		while(!fichierAutoJoin->atEnd())
		{
			temp=fichierAutoJoin->readLine();
			temp=temp.mid(0,temp.size()-1);
			if((temp.startsWith("+@") && temp.mid(2)==serveur->nom()) || (temp.startsWith("@") && temp.mid(1)==serveur->nom())) go=1;
			else if((temp.startsWith("+@") && temp.mid(2)!=serveur->nom()) || (temp.startsWith("@") && temp.mid(1)!=serveur->nom())) go=0;
			if(go && temp.startsWith("+#")) envoyerJoin(temp.mid(1),serveur);
			else if(go && !temp.startsWith("+@") && temp.startsWith("+")) envoyerMp(temp.mid(1),serveur);
		}
		fichierAutoJoin->close();
	}
}

void FenPrincipale::rejoindreLesChansOuverts(OngletServeur * serveur)
{
	QList<Onglet *>::iterator i;
	for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->type()==CHAN && qobject_cast<OngletChan*>(*i)->serveur()==serveur)
		{
			qobject_cast<OngletChan*>(*i)->connecter();
		}
	}
}

void FenPrincipale::reduire(QSystemTrayIcon::ActivationReason reason)
{
	if(reason==QSystemTrayIcon::Trigger) cacher();
}

void FenPrincipale::changementElement(QTreeWidgetItem *element)
{
	changerOnglet(ongletElementArbre(element));
}

Onglet * FenPrincipale::ongletElementArbre(QTreeWidgetItem *element)
{
	QList<Onglet *>::iterator i;
    for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->elementArbre()==element)
		{
			return *i;
		}
	}
	return NULL;
}

void FenPrincipale::changerOnglet(Onglet * onglet)
{
	if(m_ongletOuvert)
	{
		m_layoutPrincipal->removeWidget(m_ongletCourant);
		m_ongletCourant->hide();
		m_ongletCourant->setOngletCourant(false);
	}else m_ongletOuvert=true;
	if(onglet!=NULL)
	{
		m_layoutPrincipal->addWidget(onglet,10);
		m_arbre->setCurrentItem(onglet->elementArbre());
		onglet->edition()->setFocus();
		onglet->show();
		m_ongletCourant=onglet;
		m_ongletCourant->elementArbre()->setForeground(0,QBrush(QColor(0,0,0)));
		m_ongletCourant->setOngletCourant(true);
	} else m_ongletOuvert=false;
	mettreAJourTitreFenetre();
}

void FenPrincipale::mettreAJourTitreFenetre()
{
	if(!m_ongletOuvert) setWindowTitle("client_irc");
	else
	{
		OngletServeur * serveur=m_ongletCourant->type()!=SERVEUR ? qobject_cast<OngletDiscussion *>(m_ongletCourant)->serveur() : qobject_cast<OngletServeur *>(m_ongletCourant);
		setWindowTitle("client_irc: "+serveur->pseudo()+" @ "+serveur->nom()+(m_ongletCourant->type()!=SERVEUR ? (" / "+m_ongletCourant->nom()) : ""));
	}
}
void FenPrincipale::connecter_(QString serveur,QString pseudo,int port)
{
	disconnect(m_fenConnexion,SIGNAL(connecter(QString,QString,int)),this,SLOT(connecter_(QString,QString,int)));
	m_fenConnexion->close();
	connecter(serveur,pseudo,port);
}
void FenPrincipale::connecter(QString serveur)
{
	connecter(serveur,m_parametres["pseudoParDefaut"],m_parametres["portParDefaut"].toInt(),false);
}
void FenPrincipale::connecter(QString serveur,QString pseudo,int port,bool ouvertAvecAutoJoin)
{
	OngletServeur * onglet=new OngletServeur(serveur,m_ongletCss,m_emplacementDonnees,port,pseudo,ouvertAvecAutoJoin);
	m_onglet<<onglet;
	m_arbre->addTopLevelItem(onglet->elementArbre());
	m_arbre->expandItem(onglet->elementArbre());
	changerOnglet(onglet);
	connect(onglet->socket(),SIGNAL(readyRead()),this,SLOT(recevoir()));
	connect(onglet->socket(),SIGNAL(disconnected()),this,SLOT(deconnecte()));
	connect(onglet->edition(),SIGNAL(returnPressed()),this,SLOT(envoyer()));
	connect(onglet->labelPseudo(),SIGNAL(clicked()),this,SLOT(envoyerNick()));
	connect(onglet,SIGNAL(aRecevoir()),this,SLOT(recevoir_()));
}

void FenPrincipale::deconnecte()
{
	QTcpSocket *sender=qobject_cast<QTcpSocket *>(QObject::sender());
	QList<Onglet *>::iterator i;
	for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->type()==SERVEUR && qobject_cast<OngletServeur *>(*i)->socket()==sender)
		{
			QList<Onglet *>::iterator j;
			for(j=m_onglet.begin();j!=m_onglet.end();++j)
			{
				if((*j)->type()!=SERVEUR && qobject_cast<OngletDiscussion*>(*j)->serveur()==(*i))
				{
					(*j)->ecrire(tr("Déconnecté"));
				}
			}
			(*i)->ecrire(tr("Déconnecté"));
			break;
		}
	}
}

void FenPrincipale::enleverPersonne(OngletChan * numOnglet,QString pseudo)
{
	delete numOnglet->listeConnecte()->takeItem(numOnglet->listeConnecte()->row(numOnglet->listeConnecte()->findItems(pseudo,Qt::MatchExactly).first()));
}
QString FenPrincipale::ajouterPersonne(OngletChan * numOnglet,QString pseudo)
{
	QString rang=pseudo.left(1);	
	SRang srang=m_rang.rang(rang);
	int importanceRang=srang.importance,importanceRangEnCours=0;
	Connecte * item;
	bool fait=false;
	pseudo=importanceRang!=0 ? pseudo.mid(1) : pseudo;
	QIcon icone=*(srang.icone);
	item=new Connecte(icone,pseudo,srang.mode);
	for(int i=0;i<numOnglet->listeConnecte()->count();i++)
	{
		importanceRangEnCours=m_rang.mode(dynamic_cast<Connecte *>(numOnglet->listeConnecte()->item(i))->rang()).importance;
		if(importanceRangEnCours<importanceRang || (importanceRangEnCours==importanceRang && QString::localeAwareCompare(pseudo,numOnglet->listeConnecte()->item(i)->text())<0))
		{
			numOnglet->listeConnecte()->insertItem(i,item);
			fait=true;
			break;
		}
	}
	if(!fait) numOnglet->listeConnecte()->insertItem(numOnglet->listeConnecte()->count(),item);
	numOnglet->edition()->ajouterMot(pseudo);
	return pseudo;
}

void FenPrincipale::ajouterElementDiscussionArbre(OngletDiscussion * discussion)
{
	Onglet * onglet;
	int i;
	for(i=0;i<discussion->serveur()->elementArbre()->childCount();i++)
	{
		onglet=ongletElementArbre(discussion->serveur()->elementArbre()->child(i));
		if(onglet->type()>discussion->type() || (onglet->type()==discussion->type() && QString::localeAwareCompare(discussion->nom(),onglet->nom())<0)) break;
	}
	discussion->serveur()->elementArbre()->insertChild(i,discussion->elementArbre());
}

OngletDiscussion * FenPrincipale::discussion(QString nom,OngletServeur * serveur)
{
    for(QList<Onglet *>::iterator i=m_onglet.begin();i!=m_onglet.end();++i) if((*i)->nom()==nom && (*i)->type()!=SERVEUR && qobject_cast<OngletDiscussion *>(*i)->serveur()==serveur) return qobject_cast<OngletDiscussion *>(*i);
	return NULL;
}

OngletServeur * FenPrincipale::trouverServeur(QString nom)
{
	QList<Onglet *>::iterator i;
    for(i=m_onglet.begin();i!=m_onglet.end();++i) if((*i)->nom().section(".",-2,-1)==nom.section(".",-2,-1) && (*i)->type()==SERVEUR) return qobject_cast<OngletServeur *>(*i);
	return NULL;
}

void FenPrincipale::supprimerLorsqueDeconnecte()
{
	QTcpSocket *sender = qobject_cast<QTcpSocket *>(QObject::sender());
	QList<Onglet *>::iterator i;
    for(i=m_onglet.begin();i!=m_onglet.end();++i)
	{
		if((*i)->type()==SERVEUR && qobject_cast<OngletServeur *>(*i)->socket()==sender)
		{
			m_onglet.removeOne(*i);
			break;
		}
	}
}

void FenPrincipale::quitterMp(OngletMp * mp)
{
	if(m_ongletCourant==mp) changerOnglet(mp->serveur());
	mp->serveur()->elementArbre()->removeChild(mp->elementArbre());
	m_onglet.removeOne(mp);
}
void FenPrincipale::clignoter()
{
	if(!m_clignotant) m_sysTray->setIcon(*m_iconeSysTrayNouveauMessage);
	else m_sysTray->setIcon(*m_iconeSysTray);
	m_clignotant=m_clignotant ? 0 : 1;
}
