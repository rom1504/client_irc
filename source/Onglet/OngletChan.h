#ifndef DEF_ONGLETCHAN
#define DEF_ONGLETCHAN
 
#include <QtGui>
#include <QtNetwork>
#include "OngletDiscussion.h"

class Connecte : public QListWidgetItem
{
	public:
	Connecte(QIcon & icon,QString & texte,QString rang);
	Connecte(QString & texte,QString rang);
	QString rang();
	void setRang(QString rang);
	int plusHautRang();

	private:
	QString m_rang;
};

class OngletChan : public OngletDiscussion
{
	Q_OBJECT
	 
	public:
	OngletChan(QString nom,QString css,QString emplacementDonnees,OngletServeur * serveur);
	QListWidget * listeConnecte();
	QAction * actionOuvrirUnMp();
	QLineEdit * sujet();
	void connecter();
	void envoyer(QString texte);
	void augmenterNombreWhoDemande();
	void baisserNombreWhoDemande();
	bool whoDemande();
	void changerStatus(QString pseudo,bool absent);
	
	private slots:
	void who();
	void whois();
	void op();
	void deOp();
	void voice();
	void deVoice();
	void kick();
	void ban();
	void kickBan();
	
	private:
	QListWidget * m_listeConnecte;
	QAction * m_actionOuvrirUnMp;
	QAction * m_actionWhois;
	
	QMenu * m_menuActionOperateur;
	QAction * m_actionOp;
	QAction * m_actionDeOp;
	QAction * m_actionVoice;
	QAction * m_actionDeVoice;
	QAction * m_actionKick;
	QAction * m_actionBan;
	QAction * m_actionKickBan;
	QLineEdit *m_sujet;
	QTimer *m_timerWho;
	int m_nombreWhoDemande;
};

#endif
