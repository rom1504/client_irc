#ifndef DEF_FENCONNEXION
#define DEF_FENCONNEXION
 
#include <QtGui>

class FenConnexion : public QDialog
{
	Q_OBJECT
	 
	public:
	FenConnexion(QMap<QString,QString> parametres);
	
	private slots:
	void connexion();
	
	signals:
	void connecter(QString serveur,QString pseudo,int port);
	
	private:
	QHBoxLayout *m_layoutConnexion;
	QPushButton *m_boutonConnexion;
	QLabel *m_labelPseudo;
	QLabel *m_labelServeur;
	QLabel *m_labelPort;
	QLineEdit *m_champServeur;
	QLineEdit *m_champPseudo;
	QSpinBox *m_champPort;
};
 
#endif
