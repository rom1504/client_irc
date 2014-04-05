#include "FenConnexion.h"

FenConnexion::FenConnexion(QMap<QString,QString> parametres) : QDialog()
{
	setModal(true);
	setWindowIcon(QIcon(":images/icone.png"));
	setWindowTitle(tr("client_irc: Connexion"));
	
	m_boutonConnexion=new QPushButton(tr("Connexion"));
	m_layoutConnexion=new QHBoxLayout;
	m_labelPseudo=new QLabel(tr("Pseudo :"));
	m_labelServeur=new QLabel(tr("Serveur :"));
	m_labelPort=new QLabel(tr("Port :"));
	
	m_champServeur=new QLineEdit(parametres["serveurParDefaut"]);
	m_champPort=new QSpinBox;
	m_champPort->setMaximum(65536);
	m_champPort->setValue(parametres["portParDefaut"].toInt());
	m_champPseudo=new QLineEdit(parametres["pseudoParDefaut"]);
	
	
	// ajouts des widgets dans les layout
	m_layoutConnexion->addWidget(m_labelPseudo);
	m_layoutConnexion->addWidget(m_champPseudo);
	m_layoutConnexion->addWidget(m_labelServeur);
	m_layoutConnexion->addWidget(m_champServeur);
	m_layoutConnexion->addWidget(m_labelPort);
	m_layoutConnexion->addWidget(m_champPort);
	m_layoutConnexion->addWidget(m_boutonConnexion);
	setLayout(m_layoutConnexion);
	connect(m_boutonConnexion,SIGNAL(clicked()),this,SLOT(connexion()));
	show();
}

void FenConnexion::connexion()
{
	emit connecter(m_champServeur->text(),m_champPseudo->text(),m_champPort->value());
}
