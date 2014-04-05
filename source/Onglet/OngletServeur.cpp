#include "OngletServeur.h"

OngletServeur::OngletServeur(QString nom,QString css,QString emplacementDonnees,int port,QString pseudo,bool ouvertAvecAutoJoin) : Onglet(nom,css,emplacementDonnees)
{
	m_type=SERVEUR;
	m_ouvertAvecAutoJoin=ouvertAvecAutoJoin;
	m_pseudo=pseudo;
	m_port=port;
	m_socket=new QTcpSocket;
	m_pseudoAccepte=false;
	m_log=new QFile(m_emplacementDonnees+"/log/"+nom+"-"+nom+".log");
	m_log->open(QIODevice::Append|QIODevice::WriteOnly|QIODevice::Text);
	m_labelPseudo=new LabelPseudo(pseudo);
	m_layoutPseudoEdition->insertWidget(0,m_labelPseudo);
	m_timerReconnection=new QTimer;
	m_timerReconnection->setInterval(5000);
	m_timerReception=new QTimer;
	m_timerReception->setInterval(50);
	m_ferme=0;
	connect(m_timerReconnection,SIGNAL(timeout()),this,SLOT(connecter()));
	connect(m_socket,SIGNAL(hostFound()),this,SLOT(hoteTrouve()));
	connect(m_socket,SIGNAL(connected()),this,SLOT(connectionReussie()));
	connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(erreur()));
	connect(m_timerReception,SIGNAL(timeout()),this,SLOT(emettreARecevoir()));
	connecter();
	m_absent=0;
}

bool OngletServeur::absent()
{
	return m_absent;
}

void OngletServeur::ajouterAuBuffer(QByteArray ligne)
{
	if(!m_timerReception->isActive()) m_timerReception->start();
	m_buffer.enqueue(ligne);
}

QByteArray OngletServeur::prendreDuBuffer()
{
	QByteArray retour=m_buffer.dequeue();
	if(m_buffer.isEmpty()) m_timerReception->stop();
	return retour;
}

void OngletServeur::emettreARecevoir()
{
	emit aRecevoir();
}

unsigned int OngletServeur::tempsAbsence()
{
	return m_dateAbsence.secsTo(QDateTime::currentDateTime());
}

void OngletServeur::devenirAbsent()
{
	m_dateAbsence=QDateTime::currentDateTime();
	m_absent=true;
}

void OngletServeur::devenirPresent()
{
	m_absent=false;
}

void OngletServeur::setNom(QString nom)
{
	Onglet::setNom(nom);
	QFile * fichierCharset;
	QString temp="";
	fichierCharset=new QFile(m_emplacementDonnees+"/conf/charset.conf");
	fichierCharset->open(QIODevice::ReadOnly|QIODevice::Text);
	QString identifieur="@"+m_nom;
	while(!fichierCharset->atEnd())
	{
		temp=fichierCharset->readLine();
		temp=temp.mid(0,temp.size()-1);
		if(temp.section(" ",0,0)==identifieur) m_codec=QTextCodec::codecForName(temp.section(" ",1).toUtf8());
	}
	fichierCharset->close();
}

void OngletServeur::connecter()
{
	if(m_timerReconnection->isActive()) m_timerReconnection->stop();
	ecrire(tr("Recherche de l'hote ")+m_nom,"connexion");
	if(!m_ferme) m_socket->connectToHost(m_nom,m_port);
}

void OngletServeur::deconnecter()
{
	if(m_timerReconnection->isActive()) m_timerReconnection->stop();
	m_socket->disconnectFromHost();
	m_ferme=1;
}

void OngletServeur::reconnecter()
{
	m_timerReception->stop();
	m_buffer.clear();
	if(m_socket->state()==QAbstractSocket::ConnectedState) m_socket->disconnectFromHost();
	if(!m_timerReconnection->isActive() && !m_ferme) m_timerReconnection->start();
}

void OngletServeur::hoteTrouve()
{
	ecrire(tr("Connection à ")+m_nom+":"+QString::number(m_port)+tr(" ..."),"connexion");
}

void OngletServeur::connectionReussie()
{
	ecrire(tr("Connecté. Maintenant identification..."),"connexion");
	envoyer("USER "+m_pseudo+" localhost "+m_nom+" :"+m_pseudo);
	envoyer("NICK "+m_pseudo);
	m_pseudoAccepte=false;
}

void OngletServeur::erreur()
{
	ecrire(tr("Connection échouée. Erreur : ")+m_socket->errorString(),"erreur");
	if(!m_timerReconnection->isActive() && !m_ferme) m_timerReconnection->start();
}


QString OngletServeur::pseudo()
{
	return m_pseudo;
}

bool OngletServeur::isOuvertAvecAutoJoin()
{
	return m_ouvertAvecAutoJoin;
}

void OngletServeur::setPseudo(QString pseudo)
{
	m_pseudo=pseudo;
}

bool OngletServeur::pseudoAccepte()
{
	return m_pseudoAccepte;
}

void OngletServeur::setPseudoAccepte(bool pseudoAccepte)
{
	m_pseudoAccepte=pseudoAccepte;
}

QTcpSocket *OngletServeur::socket()
{
	return m_socket;
}

void OngletServeur::envoyer_(QByteArray texte)
{
	if(m_socket->state()==QAbstractSocket::ConnectedState) m_socket->write(texte+"\r\n");
}

void OngletServeur::envoyer(QString texte)
{
	envoyer_(m_codec->fromUnicode(texte));
}
