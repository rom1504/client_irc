#ifndef DEF_FENFAVORIS
#define DEF_FENFAVORIS
 
#include <QtGui>

class FenEditionFavoris : public QDialog
{
	Q_OBJECT
	
	public://constructeur
	FenEditionFavoris(QTreeWidgetItem * element,QWidget * parent);
	
	signals://signaux
	void termine();
	void annule(QTreeWidgetItem * element);
	
	protected slots://slots protégés
	void terminer();
	
	protected://fonction protégées
	void closeEvent(QCloseEvent * event);
	
	protected:
	QVBoxLayout * m_layoutPrincipal;
	QFormLayout * m_layoutFormulaire;
	QHBoxLayout * m_layoutBouton;
	QTreeWidgetItem * m_element;
	QLineEdit * m_nom;
	QCheckBox * m_autoJoin;
	QPushButton * m_boutonOk;
	QPushButton * m_boutonAnnuler;
	bool m_termine;
};

class FenEditionServeurFavoris : public FenEditionFavoris
{
	Q_OBJECT
	
	public://constructeur
	FenEditionServeurFavoris(QTreeWidgetItem * element,QWidget * parent);
};

class FenEditionDiscussionFavoris : public FenEditionFavoris
{
	Q_OBJECT
	
	public://constructeur
	FenEditionDiscussionFavoris(QTreeWidgetItem * element,QWidget * parent);
};

class FenFavoris : public QDialog
{
	Q_OBJECT
	 
	public://constructeur
	FenFavoris(QString emplacementDonnees);
	
	signals://signaux
	void connection(QString serveur);
	void join(QString discussion,QString serveur);
	
	private slots://slots privés
	void connecter();
	void supprimer();
	void ajouterServeur();
	void ajouterDiscussion();
	void annulerAjouter(QTreeWidgetItem * element);
	void editer();
	void nePasEnregistrer();
	
	private://fonction privées
	void enregistrer();
	void charger();
	void closeEvent(QCloseEvent * event);
	void supprimer(QTreeWidgetItem * element);
	
	private://diverses  variables
	QVBoxLayout * m_layoutPrincipal;
	QHBoxLayout * m_layoutBouton;
	QTreeWidget * m_favoris;
	QList<QTreeWidgetItem *> m_listeFavoris;
	QPushButton * m_boutonConnecter;
	QPushButton * m_boutonAjouterServeur;
	QPushButton * m_boutonAjouterDiscussion;
	QPushButton * m_boutonEditer;
	QPushButton * m_boutonSupprimer;
	QPushButton * m_boutonEnregistrer;
	QPushButton * m_boutonFermerSansEnregistrer;
	bool m_enregistrerALaFermeture;
	QString m_emplacementDonnees;
	
	FenEditionServeurFavoris * m_fenEditionServeurFavoris;
	FenEditionDiscussionFavoris * m_fenEditionDiscussionFavoris;
};
 
#endif
