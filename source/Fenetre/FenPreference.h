#ifndef DEF_FENPREFERENCE
#define DEF_FENPREFERENCE
 
#include <QtGui>

class FenPreference : public QDialog
{
	Q_OBJECT
	 
	public:
	FenPreference(QMap<QString,QString> * parametres,QString emplacementDonnees);
	
	signals:
	void parametreChange();
	
	private slots:
	void changementElement(QTreeWidgetItem *element);
	void enregistrer();
	void changerCouleur();
	void revenirAuDefaut();
	void changerEmplacementDonnees_();
	void changerEmplacementDonnees();
	
	private://fonction privées
	void ajouterCouleur(QString nom,QString texteBouton,QString texteFormulaire);
	void enregistrerFichier();
	
	private:
	QMap<QString,QString> *m_parametres;
	QVBoxLayout * m_layoutPrincipal;
	QHBoxLayout * m_layoutArbrePage;
	QHBoxLayout * m_layoutBouton;
	QList <QTreeWidgetItem *>m_element;
	QList <QWidget *> m_onglet;
	QTreeWidget *m_arbre;
	bool m_ongletOuvert;
	QWidget * m_ongletCourant;
	void changerOnglet(QWidget * onglet);
	QTreeWidgetItem *m_elementInterface;
	QPushButton * m_boutonOk;
	QPushButton * m_boutonAnnuler;
	QPushButton * m_boutonDefaut;
	QString m_emplacementDonnees;
	
	//page Divers
	QWidget * m_pageDivers;
	QFormLayout *m_accueilLayoutFormulaire;
	QLineEdit * m_serveurParDefaut;
	QSpinBox * m_portParDefaut;
	QLineEdit * m_pseudoParDefaut;
	QCheckBox * m_autoJoin;
	QCheckBox * m_superAutoJoin;
	QLineEdit * m_quitMessageParDefaut;
	QLineEdit * m_awayMessageParDefaut;
	QCheckBox * m_nePasQuitter;
	QCheckBox * m_notificationsDesactives;
	QCheckBox * m_cacherFenetrePrincipaleAuDemarrage;
	QHBoxLayout * m_layoutEmplacementDonnees;
	QPushButton * m_boutonEmplacementDonnees;
	QLineEdit * m_champEmplacementDonnees;
	QFileDialog * m_fenetreChoixEmplacementDonnees;
	
	//page Couleur
	QWidget * m_pageCouleur;
	QFormLayout * m_couleurLayoutFormulaire;
	QMap <QString,QPushButton *> m_couleur;
	QPushButton * m_couleurJoin;
	QPushButton * m_couleurPart;
	QPushButton * m_couleurQuit;
};
 
#endif
