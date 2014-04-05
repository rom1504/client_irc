#ifndef DEF_ONGLETMP
#define DEF_ONGLETMP
 
#include <QtGui>
#include <QtNetwork>
#include "OngletDiscussion.h"

class OngletMp : public OngletDiscussion
{
	Q_OBJECT
	 
	public:
	OngletMp(QString nom,QString css,QString emplacementDonnees,OngletServeur * serveur);
	void envoyer(QString texte);
};

#endif
