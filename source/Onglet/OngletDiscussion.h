#ifndef DEF_ONGLETDISCUSSION
#define DEF_ONGLETDISCUSSION
 
#include <QtGui>
#include <QtNetwork>
#include "Onglet.h"
#include "OngletServeur.h"

class OngletDiscussion : public Onglet
{
	Q_OBJECT
	
	public:
	OngletDiscussion(QString nom,QString css,QString emplacementDonnees,OngletServeur * serveur);
	OngletServeur * serveur();
	
	protected:
	OngletServeur *m_serveur;
};

#endif
