#ifndef PLUGINIA
#define PLUGINIA
 
#include <QtGui>
#include <QtNetwork>
#include "Plugin.h"

class PluginIA : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(Plugin)
	
	public:
	~PluginIA();
	void initialiser();
	QAction * action();
	QString reponse(QString message);
	void messageRecu(QString pseudo,QString emetteur,QString serveur,QString message);
	
	signals:
	void messageReponse(QString emetteur,QString serveur,QString message);
	
	private slots:
	void donneesRecues();
	void reconnecter();
	void connecter();
	
	private:
	QAction * m_action;
	QTcpSocket * m_socket;
	QTimer * m_timerReconnection;
};

#endif
