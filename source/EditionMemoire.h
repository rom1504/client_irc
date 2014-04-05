#ifndef DEF_EDITIONMEMOIRE
#define DEF_EDITIONMEMOIRE
 
#include <QtGui>

class EditionMemoire : public QLineEdit
{
	Q_OBJECT
	 
	public:
	EditionMemoire();//constructeur
	void ajouterTexte(QString texte);
	
	private:
	void keyPressEvent(QKeyEvent * e);
	
	private://variables diverses
	int m_texteEnCours;
	QStringList m_listeTexte;
};
 
#endif
