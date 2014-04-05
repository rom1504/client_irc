#ifndef DEF_ONGLETSERVEUR
#define DEF_ONGLETSERVEUR
 
#include <QtGui>
#include <QtNetwork>
#include "Onglet.h"

class OngletServeur : public Onglet
{
	Q_OBJECT
	 
	public:
	OngletServeur(QString nom,QString css,QString emplacementDonnees,int port,QString pseudo,bool ouvertAvecAutoJoin=false);
	void envoyer(QString texte);
	void envoyer_(QByteArray texte);
	bool isOuvertAvecAutoJoin();
	QTcpSocket *socket();
	QString pseudo();
	void setPseudo(QString pseudo);
	int port();
	void setPort(int port);
	void reconnecter();
	void deconnecter();
	bool pseudoAccepte();
	void setPseudoAccepte(bool pseudoAccepte);
	void setNom(QString nom);
	bool absent();
	unsigned int tempsAbsence();
	void devenirAbsent();
	void devenirPresent();
	void ajouterAuBuffer(QByteArray ligne);
	QByteArray prendreDuBuffer();
	
	signals:
	void aRecevoir();
	
	private slots:
	void connectionReussie();
	void hoteTrouve();
	void erreur();
	void connecter();
	void emettreARecevoir();
	
	private:
	bool m_pseudoAccepte;
	bool m_ouvertAvecAutoJoin;
	QTcpSocket *m_socket;
	QString m_pseudo;
	int m_port;
	QTimer * m_timerReconnection;
	bool m_ferme;
	QDateTime m_dateAbsence;
	bool m_absent;
	QQueue<QByteArray> m_buffer;
	QTimer * m_timerReception;
};

#endif
