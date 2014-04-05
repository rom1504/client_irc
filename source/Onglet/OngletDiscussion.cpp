#include "OngletDiscussion.h"

OngletDiscussion::OngletDiscussion(QString nom,QString css,QString emplacementDonnees,OngletServeur * serveur) : Onglet(nom,css,emplacementDonnees)
{
	m_serveur=serveur;
	m_codec=serveur->codec();
	m_log=new QFile(m_emplacementDonnees+"/log/"+serveur->nom()+"-"+nom+".log");
	m_log->open(QIODevice::Append|QIODevice::WriteOnly|QIODevice::Text);
	m_labelPseudo=new LabelPseudo(serveur->pseudo());
	m_layoutPseudoEdition->insertWidget(0,m_labelPseudo);
}

OngletServeur * OngletDiscussion::serveur()
{
	return m_serveur;
}
