#include "OngletMp.h"

OngletMp::OngletMp(QString nom,QString css,QString emplacementDonnees,OngletServeur * serveur) : OngletDiscussion(nom,css,emplacementDonnees,serveur)
{
	m_type=MP;
}

void OngletMp::envoyer(QString texte)
{
	m_serveur->envoyer(texte);
}
