#ifndef DEF_PLUGIN
#define DEF_PLUGIN
 
#include <QtGui>
#include <QtNetwork>

class Plugin : public QObject
{
	public:
	virtual ~Plugin() {}
	virtual void initialiser()=0;
	virtual QAction * action()=0;
	virtual QString reponse(QString message)=0;
	virtual void messageRecu(QString pseudo,QString emetteur,QString serveur,QString message)=0;
};

Q_DECLARE_INTERFACE(Plugin,"client_irc.Plugin/0.3")

#endif
