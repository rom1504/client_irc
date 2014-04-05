#include "FenPreference.h"

FenPreference::FenPreference(QMap<QString,QString> * parametres,QString emplacementDonnees) : QDialog(),m_emplacementDonnees(emplacementDonnees)
{
	//divers
	m_parametres=parametres;
	setModal(true);
	setMinimumSize(700,400);
	setWindowIcon(QIcon(":images/icone.png"));
	setWindowTitle(tr("client_irc: Préférence"));
	QTreeWidgetItem * temp;
	
	//création widgets généraux
	m_layoutPrincipal=new QVBoxLayout;
	m_layoutPrincipal->setAlignment(Qt::AlignLeft);
	m_layoutArbrePage=new QHBoxLayout;
	m_arbre=new QTreeWidget;
	m_arbre->headerItem()->setHidden(true);
	m_layoutArbrePage->addWidget(m_arbre,1);
	m_ongletCourant=NULL;
	m_ongletOuvert=false;
	m_elementInterface=new QTreeWidgetItem(QStringList(tr("Interface")));
	m_arbre->addTopLevelItem(m_elementInterface);
	m_arbre->expandItem(m_elementInterface);
	
	//page divers
	m_pageDivers=new QWidget;
	m_accueilLayoutFormulaire=new QFormLayout;
	m_serveurParDefaut=new QLineEdit((*m_parametres)["serveurParDefaut"]);
	m_portParDefaut=new QSpinBox;
	m_portParDefaut->setMaximum(65536);
	m_portParDefaut->setValue((*m_parametres)["portParDefaut"].toInt());
	m_pseudoParDefaut=new QLineEdit((*m_parametres)["pseudoParDefaut"]);
	m_autoJoin=new QCheckBox(tr("Autojoin"));
	m_autoJoin->setChecked((*m_parametres)["autoJoin"].toInt()==1);
	m_superAutoJoin=new QCheckBox(tr("Autojoin des chans si on se connecte manuellement au serveur"));
	m_superAutoJoin->setChecked((*m_parametres)["superAutoJoin"].toInt()==1);
	m_quitMessageParDefaut=new QLineEdit((*m_parametres)["quitMessageParDefaut"]);
	m_awayMessageParDefaut=new QLineEdit((*m_parametres)["awayMessageParDefaut"]);
	
	m_nePasQuitter=new QCheckBox(tr("Ne pas quitter lorsque on ferme la fenetre mais cacher celle-ci"));
	m_nePasQuitter->setChecked((*m_parametres)["nePasQuitter"].toInt()==1);
	
	m_notificationsDesactives=new QCheckBox(tr("Notifications désactivées"));
	m_notificationsDesactives->setChecked((*m_parametres)["notificationsDesactives"].toInt()==1);
	
	m_cacherFenetrePrincipaleAuDemarrage=new QCheckBox(tr("Cacher la fenêtre principale au démarrage"));
	m_cacherFenetrePrincipaleAuDemarrage->setChecked((*m_parametres)["cacherFenetrePrincipaleAuDemarrage"].toInt()==1);
	
	m_layoutEmplacementDonnees=new QHBoxLayout;
	m_boutonEmplacementDonnees=new QPushButton(tr("&Parcourir"));
	m_champEmplacementDonnees=new QLineEdit(m_emplacementDonnees);
	m_layoutEmplacementDonnees->addWidget(m_champEmplacementDonnees);
	m_layoutEmplacementDonnees->addWidget(m_boutonEmplacementDonnees);
	connect(m_boutonEmplacementDonnees,SIGNAL(pressed()),this,SLOT(changerEmplacementDonnees_()));
	m_accueilLayoutFormulaire->addRow(tr("Serveur par défaut :"),m_serveurParDefaut);
	m_accueilLayoutFormulaire->addRow(tr("Port par défaut :"),m_portParDefaut);
	m_accueilLayoutFormulaire->addRow(tr("Pseudo par défaut :"),m_pseudoParDefaut);
	m_accueilLayoutFormulaire->addRow(m_autoJoin);
	m_accueilLayoutFormulaire->addRow(m_superAutoJoin);
	m_accueilLayoutFormulaire->addRow(tr("Quit message par défaut :"),m_quitMessageParDefaut);
	m_accueilLayoutFormulaire->addRow(tr("Away message par défaut :"),m_awayMessageParDefaut);
	m_accueilLayoutFormulaire->addRow(m_nePasQuitter);
	m_accueilLayoutFormulaire->addRow(m_notificationsDesactives);
	m_accueilLayoutFormulaire->addRow(m_cacherFenetrePrincipaleAuDemarrage);
	m_accueilLayoutFormulaire->addRow(tr("Emplacement des données ( logs, configs ),\n ne changera qu'après redémarrage du client :"),m_layoutEmplacementDonnees);

	m_pageDivers->setLayout(m_accueilLayoutFormulaire);
	m_onglet<<m_pageDivers;
	temp=new QTreeWidgetItem(QStringList(tr("Divers")));
	m_elementInterface->addChild(temp);
	m_element<<temp;
	
	//page couleur
	m_pageCouleur=new QWidget;
	m_couleurLayoutFormulaire=new QFormLayout;
	ajouterCouleur("couleurJoin",tr("toto(toto!toto@totohost) a rejoint #toto"),tr("Couleur des messages de join :"));
	ajouterCouleur("couleurPart",tr("toto(toto!toto@totohost) a quitté #toto"),tr("Couleur des messages de part :"));
	ajouterCouleur("couleurQuit",tr("toto(toto!toto@totohost) a quitté totoNetwork"),tr("Couleur des messages de quit :"));
	ajouterCouleur("couleurKick",tr("toto a kické toto de #toto (toto)"),tr("Couleur des messages de kick :"));
	ajouterCouleur("couleurTopic",tr("Le sujet pour #toto est : toto"),tr("Couleur des messages de topic :"));
	
	m_pageCouleur->setLayout(m_couleurLayoutFormulaire);
	m_onglet<<m_pageCouleur;
	temp=new QTreeWidgetItem(QStringList(tr("Couleur")));
	m_elementInterface->addChild(temp);
	m_element<<temp;
	
	// affichage fenetre
	m_layoutPrincipal->addLayout(m_layoutArbrePage);
	m_boutonOk=new QPushButton(tr("Ok"));
	m_boutonOk->setAutoDefault(true);
	m_boutonAnnuler=new QPushButton(tr("Annuler"));
	m_boutonDefaut=new QPushButton(tr("Revenir aux parametres par defaut"));
	m_layoutBouton=new QHBoxLayout;
	m_layoutBouton->setAlignment(Qt::AlignRight);
	m_layoutBouton->addWidget(m_boutonOk);
	m_layoutBouton->addWidget(m_boutonAnnuler);
	m_layoutBouton->addWidget(m_boutonDefaut);
	m_layoutPrincipal->addLayout(m_layoutBouton);
	setLayout(m_layoutPrincipal);
	show();
	
	//connection
	connect(m_arbre,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(changementElement(QTreeWidgetItem*)));
	connect(m_boutonOk,SIGNAL(clicked()),this,SLOT(enregistrer()));
	connect(m_boutonAnnuler,SIGNAL(clicked()),this,SLOT(close()));
	connect(m_boutonDefaut,SIGNAL(clicked()),this,SLOT(revenirAuDefaut()));
	
	//divers
	changerOnglet(m_pageDivers);
}
void FenPreference::changerEmplacementDonnees_()
{
	m_fenetreChoixEmplacementDonnees=new QFileDialog(this,tr("Emplacement données"),m_champEmplacementDonnees->text());
	m_fenetreChoixEmplacementDonnees->setOption(QFileDialog::ShowDirsOnly);
	m_fenetreChoixEmplacementDonnees->setFileMode(QFileDialog::Directory);
	connect(m_fenetreChoixEmplacementDonnees,SIGNAL(accepted()),this,SLOT(changerEmplacementDonnees()));
	m_fenetreChoixEmplacementDonnees->show();
}
void FenPreference::changerEmplacementDonnees()
{
	m_champEmplacementDonnees->setText(m_fenetreChoixEmplacementDonnees->selectedFiles().first());
}
void FenPreference::ajouterCouleur(QString nom,QString texteBouton,QString texteFormulaire)
{
	QPushButton * bouton=new QPushButton(texteBouton);
	bouton->setStyleSheet("color:"+(*m_parametres)[nom]+";");
	connect(bouton,SIGNAL(clicked()),this,SLOT(changerCouleur()));
	m_couleurLayoutFormulaire->addRow(texteFormulaire,bouton);
	m_couleur[nom]=bouton;
}
void FenPreference::changerCouleur()
{
	QPushButton *sender=qobject_cast<QPushButton *>(QObject::sender());
	QColor temp=QColorDialog::getColor(QColor(sender->styleSheet().mid(6,sender->styleSheet().size()-7)));
	if(temp.isValid()) sender->setStyleSheet("color:"+temp.name()+";");
}

void FenPreference::revenirAuDefaut()
{
	QFile * fichierConf;
	QString temp;
	fichierConf=new QFile(m_emplacementDonnees+"/conf/client_ircDefaut.conf");
	fichierConf->open(QIODevice::ReadOnly|QIODevice::Text);
	while(!fichierConf->atEnd())
	{
		temp=fichierConf->readLine();
		temp=temp.mid(0,temp.size()-1);
		(*m_parametres)[temp.section("=",0,0)]=temp.section("=",1,1);
	}
	fichierConf->close();
	enregistrerFichier();
	close();
}
void FenPreference::enregistrer()
{
	//mise des infos dans (*m_parametres)
	(*m_parametres)["serveurParDefaut"]=m_serveurParDefaut->text();
	(*m_parametres)["portParDefaut"]=QString::number(m_portParDefaut->value());
	(*m_parametres)["pseudoParDefaut"]=m_pseudoParDefaut->text();
	(*m_parametres)["autoJoin"]=m_autoJoin->isChecked() ? "1" : "0";
	(*m_parametres)["superAutoJoin"]=m_superAutoJoin->isChecked() ? "1" : "0";
	(*m_parametres)["quitMessageParDefaut"]=m_quitMessageParDefaut->text();
	(*m_parametres)["awayMessageParDefaut"]=m_awayMessageParDefaut->text();
	(*m_parametres)["nePasQuitter"]=m_nePasQuitter->isChecked() ? "1" : "0";
	(*m_parametres)["notificationsDesactives"]=m_notificationsDesactives->isChecked() ? "1" : "0";
	(*m_parametres)["cacherFenetrePrincipaleAuDemarrage"]=m_cacherFenetrePrincipaleAuDemarrage->isChecked() ? "1" : "0";
	for(QMap<QString,QPushButton *>::iterator i=m_couleur.begin();i!=m_couleur.end();++i) (*m_parametres)[i.key()]=(*i)->styleSheet().mid(6,(*i)->styleSheet().size()-7);
	
	//enregistrement de emplacementDonnees
	QStringList args=QCoreApplication::arguments();
	QFile * fichierEmplacementDonnees=new QFile((args.size()>1 ? args.at(1) : QDir::homePath()+"/.client_irc")+"/conf/emplacementDonnees.conf");
	fichierEmplacementDonnees->open(QIODevice::WriteOnly | QIODevice::Text);
	fichierEmplacementDonnees->write(m_champEmplacementDonnees->text().toUtf8());
	fichierEmplacementDonnees->close();
	
	//enregistrement dans le fichier
	enregistrerFichier();
	emit parametreChange();//utilisé par FenPrincipale pour mettre à jour le css des onglet par rapport aux couleur
	close();
}
void FenPreference::enregistrerFichier()
{
	QFile * fichierConf;
	fichierConf=new QFile(m_emplacementDonnees+"/conf/client_irc.conf");
	fichierConf->open(QIODevice::WriteOnly|QIODevice::Text);
	QTextStream streamConf(fichierConf);
	for(QMap<QString,QString>::iterator j=(*m_parametres).begin();j!=(*m_parametres).end();++j) streamConf<<j.key()<<"="<<(*j)<<"\n";
	fichierConf->close();
}
void FenPreference::changerOnglet(QWidget * onglet)
{
	if(m_ongletOuvert)
	{
		m_layoutArbrePage->removeWidget(m_ongletCourant);
		m_ongletCourant->hide();
	}
	else m_ongletOuvert=true;
	if(onglet!=NULL)
	{
		m_layoutArbrePage->addWidget(onglet,5);
		m_arbre->setCurrentItem(m_element[m_onglet.indexOf(onglet)]);
		onglet->show();
		m_ongletCourant=onglet;
	}
	else m_ongletOuvert=false;
}
void FenPreference::changementElement(QTreeWidgetItem *element)
{
	if(element!=m_elementInterface) changerOnglet(m_onglet[m_element.indexOf(element)]);
}
