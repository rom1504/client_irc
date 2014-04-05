#include "PluginSimple.h"

Q_EXPORT_PLUGIN2(PluginSimple,PluginSimple)

void PluginSimple::initialiser()
{
	m_action=new QAction("PluginSimple",this);
	connect(m_action,SIGNAL(triggered()),this,SLOT(ouvrirFenetre()));
	m_fenetre=NULL;
	m_reponse="Plop, je m'appelle PluginSimple :)";
}

void PluginSimple::messageRecu(QString pseudo,QString emetteur,QString serveur,QString message)
{

}

void PluginSimple::ouvrirFenetre()
{
	m_fenetre=new QWidget;
	QVBoxLayout * layout=new QVBoxLayout;
	m_fenetre->setLayout(layout);
	m_editionReponse=new QLineEdit(m_reponse,m_fenetre);
	m_boutonOk=new QPushButton("Ok",m_fenetre);
	layout->addWidget(m_editionReponse);
	layout->addWidget(m_boutonOk);
	m_fenetre->show();
	connect(m_boutonOk,SIGNAL(pressed()),this,SLOT(modifierReponse()));
}

void PluginSimple::modifierReponse()
{
	m_reponse=m_editionReponse->text();
}

QString PluginSimple::reponse(QString message)
{
	return message=="!PluginSimple" ? m_reponse  : "";
}

QAction * PluginSimple::action()
{
	return m_action;
}

PluginSimple::~PluginSimple()
{
	if(m_fenetre!=NULL) delete m_fenetre;
}
