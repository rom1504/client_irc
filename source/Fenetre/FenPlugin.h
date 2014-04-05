#ifndef DEF_FENPLUGIN
#define DEF_FENPLUGIN
 
#include <QtGui>

class FenPlugin : public QDialog
{
	Q_OBJECT
	 
	public://constructeur
	FenPlugin(QString emplacementDonnees);
	
	private://fonction privées
	void enregistrer();
	void charger();
	void closeEvent(QCloseEvent * event);
	
	private://diverses  variables
	QVBoxLayout * m_layoutPrincipal;
	QTableWidget * m_listePlugin;
	QString m_emplacementDonnees;
};
 
#endif
