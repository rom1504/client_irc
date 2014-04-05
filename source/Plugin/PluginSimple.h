#ifndef PLUGINSIMPLE
#define PLUGINSIMPLE
 
#include <QtGui>
#include <QtNetwork>
#include "Plugin.h"

class PluginSimple : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(Plugin)
	
	public:
	~PluginSimple();
	void initialiser();
	QAction * action();
	QString reponse(QString message);
	void messageRecu(QString pseudo,QString emetteur,QString serveur,QString message);
	
	private slots:
	void ouvrirFenetre();
	void modifierReponse();
	
	private:
	QWidget * m_fenetre;
	QAction * m_action;
	QPushButton * m_boutonOk;
	QLineEdit * m_editionReponse;
	QString m_reponse;
};

#endif
