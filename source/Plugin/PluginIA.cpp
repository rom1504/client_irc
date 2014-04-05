#include "PluginIA.h"

Q_EXPORT_PLUGIN2(PluginIA,PluginIA)

void PluginIA::initialiser()
{
	m_action=new QAction("PluginIA",this);
	m_timerReconnection=new QTimer;
	m_timerReconnection->setInterval(5000);
	m_socket=new QTcpSocket;
	connecter();
	connect(m_timerReconnection,SIGNAL(timeout()),this,SLOT(connecter()));
	connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(reconnecter()));
	connect(m_socket,SIGNAL(disconnected()),this,SLOT(reconnecter()));
	connect(m_socket,SIGNAL(readyRead()),this,SLOT(donneesRecues()));
}

void PluginIA::donneesRecues()
{
	QString recu,emetteur,serveur,message;
	while(m_socket->canReadLine())
	{
		recu=QString::fromUtf8(m_socket->readLine());
		recu=recu.mid(0,recu.size()-1);
		emetteur=recu.section(":",0,0).section("-",1,1);
		serveur=recu.section(":",0,0).section("-",2,2);
		message=recu.section(":",1,1);
		emit messageReponse(emetteur,serveur,message);
	}
}

void PluginIA::messageRecu(QString pseudo,QString emetteur,QString serveur,QString message)
{
	m_socket->write((pseudo+":irc-"+emetteur+"-"+serveur+":"+message+"\n").toUtf8());
}

void PluginIA::connecter()
{
	if(m_timerReconnection->isActive()) m_timerReconnection->stop();
	m_socket->connectToHost("127.0.0.1",32816);
}

void PluginIA::reconnecter()
{
	m_timerReconnection->start();
}

QString PluginIA::reponse(QString message)
{
	return "";
}

QAction * PluginIA::action()
{
	return m_action;
}

PluginIA::~PluginIA()
{
	
}
