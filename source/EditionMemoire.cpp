#include "EditionMemoire.h"

EditionMemoire::EditionMemoire() : QLineEdit()
{
	m_texteEnCours=0;
}

void EditionMemoire::ajouterTexte(QString texte)
{
	m_listeTexte<<texte;
	m_texteEnCours=m_listeTexte.size();
}

void EditionMemoire::keyPressEvent(QKeyEvent * e)
{
	QLineEdit::keyPressEvent(e);
	if(e->key()==Qt::Key_Up || e->key()==Qt::Key_Down)
	{
		if(e->key()==Qt::Key_Up) m_texteEnCours--;
		else if(e->key()==Qt::Key_Down) m_texteEnCours++;
		if(m_texteEnCours>=m_listeTexte.size()) m_texteEnCours=m_listeTexte.size()-1;
		else if(m_texteEnCours<0) m_texteEnCours=0;
		else if(!m_listeTexte.isEmpty()) setText(m_listeTexte[m_texteEnCours]);
	}
}
