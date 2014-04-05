#include "OngletChan.h"

Connecte::Connecte(QIcon & icon,QString & texte,QString rang) : QListWidgetItem(icon,texte)
{
	m_rang=rang;
}

Connecte::Connecte(QString & texte,QString rang) : QListWidgetItem(texte)
{
	m_rang=rang;
}

QString Connecte::rang()
{
	return m_rang;
}

int Connecte::plusHautRang() // à revoir au moment de l'implémentation de la gestion du multi-mode/multi-rang
{
	int rang=0;
	for(int i=0;i<m_rang.count();i++)
	{
		if(m_rang[i]=='o') rang=3;
		else if(m_rang[i]=='v' && rang==0) rang=1;
	}
	return rang;
}

void Connecte::setRang(QString rang)
{
	m_rang=rang;
}

OngletChan::OngletChan(QString nom,QString css,QString emplacementDonnees,OngletServeur * serveur) : OngletDiscussion(nom,css,emplacementDonnees,serveur)
{
	m_type=CHAN;
	QFile * fichierCharset;
	QString temp="";
	fichierCharset=new QFile(m_emplacementDonnees+"/conf/charset.conf");
	fichierCharset->open(QIODevice::ReadOnly|QIODevice::Text);
	QString identifieur=m_nom+"@"+m_serveur->nom();
	while(!fichierCharset->atEnd())
	{
		temp=fichierCharset->readLine();
		temp=temp.mid(0,temp.size()-1);
		if(temp.section(" ",0,0)==identifieur) m_codec=QTextCodec::codecForName(temp.section(" ",1).toUtf8());
	}
	fichierCharset->close();
	m_listeConnecte=new QListWidget;
	m_listeConnecte->setSortingEnabled(false);
	m_layoutDiscussionListe->addWidget(m_listeConnecte,1);
	m_listeConnecte->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_actionOuvrirUnMp=new QAction(tr("&Ouvrir une fenetre de discussion"),this);
	m_listeConnecte->addAction(m_actionOuvrirUnMp);
	m_actionWhois=new QAction(tr("&Information(whois)"),this);
	m_listeConnecte->addAction(m_actionWhois);
	m_menuActionOperateur=new QMenu(tr("Action &operateur"));
	m_listeConnecte->addAction(m_menuActionOperateur->menuAction());
	m_actionOp=m_menuActionOperateur->addAction(tr("Donner l'état d'opérateur"));
	m_actionDeOp=m_menuActionOperateur->addAction(tr("Prendre l'état d'opérateur"));
	m_menuActionOperateur->addSeparator();
	m_actionVoice=m_menuActionOperateur->addAction(tr("Donner la parole"));
	m_actionDeVoice=m_menuActionOperateur->addAction(tr("Prendre la parole"));
	m_menuActionOperateur->addSeparator();
	m_actionKick=m_menuActionOperateur->addAction(tr("Expulser"));
	m_actionBan=m_menuActionOperateur->addAction(tr("Bannir"));
	m_actionKickBan=m_menuActionOperateur->addAction(tr("ExpulserBannir"));
	m_sujet=new QLineEdit;
	m_sujet->setToolTip(tr("Aucun sujet n'est défini"));
	m_layoutLectureEdition->insertWidget(0,m_sujet);
	m_timerWho=new QTimer;
	m_timerWho->setInterval(20000);
	m_timerWho->start();
	connect(m_timerWho,SIGNAL(timeout()),this,SLOT(who()));
	connect(m_actionWhois,SIGNAL(triggered()),this,SLOT(whois()));
	connect(m_actionOp,SIGNAL(triggered()),this,SLOT(op()));
	connect(m_actionDeOp,SIGNAL(triggered()),this,SLOT(deOp()));
	connect(m_actionVoice,SIGNAL(triggered()),this,SLOT(voice()));
	connect(m_actionDeVoice,SIGNAL(triggered()),this,SLOT(deVoice()));
	connect(m_actionKick,SIGNAL(triggered()),this,SLOT(kick()));
	connect(m_actionBan,SIGNAL(triggered()),this,SLOT(ban()));
	connect(m_actionKickBan,SIGNAL(triggered()),this,SLOT(kickBan()));
	m_nombreWhoDemande=0;
	who();
}

void OngletChan::changerStatus(QString pseudo,bool absent)
{
	m_listeConnecte->findItems(pseudo,Qt::MatchExactly).first()->setForeground(QBrush(absent ? QColor(200,200,200) : QColor(0,0,0)));
}

void OngletChan::augmenterNombreWhoDemande()
{
	m_nombreWhoDemande++;
}

void OngletChan::baisserNombreWhoDemande()
{
	m_nombreWhoDemande--;
}

bool OngletChan::whoDemande()
{
	return m_nombreWhoDemande>=1;
}

void OngletChan::who()
{
	envoyer("WHO "+m_nom);
}

void OngletChan::connecter()
{
	m_listeConnecte->clear();
	m_serveur->envoyer("JOIN "+m_nom);
}

QAction *OngletChan::actionOuvrirUnMp()
{
	return m_actionOuvrirUnMp;
}

void OngletChan::whois()
{
	m_serveur->envoyer("WHOIS "+m_listeConnecte->currentItem()->text());
}

void OngletChan::op()
{
	m_serveur->envoyer("MODE "+m_nom+" +o "+m_listeConnecte->currentItem()->text());
}

void OngletChan::deOp()
{
	m_serveur->envoyer("MODE "+m_nom+" -o "+m_listeConnecte->currentItem()->text());
}

void OngletChan::voice()
{
	m_serveur->envoyer("MODE "+m_nom+" +v "+m_listeConnecte->currentItem()->text());
}

void OngletChan::deVoice()
{
	m_serveur->envoyer("MODE "+m_nom+" -v "+m_listeConnecte->currentItem()->text());
}

void OngletChan::kick()
{
	m_serveur->envoyer("KICK "+m_nom+" "+m_listeConnecte->currentItem()->text());
}

void OngletChan::ban()
{
	m_serveur->envoyer("MODE "+m_nom+" +b "+m_listeConnecte->currentItem()->text());
}

void OngletChan::kickBan()
{
	kick();
	ban();
}

QLineEdit * OngletChan::sujet()
{
	return m_sujet;
}

QListWidget * OngletChan::listeConnecte()
{
	return m_listeConnecte;
}

void OngletChan::envoyer(QString texte)
{
	m_serveur->envoyer_(m_codec->fromUnicode(texte));
}
