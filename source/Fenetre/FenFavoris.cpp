#include "FenFavoris.h"

FenFavoris::FenFavoris(QString emplacementDonnees) : QDialog(),m_emplacementDonnees(emplacementDonnees)
{
	//divers
	setModal(true);
	setMinimumSize(700,400);
	setWindowIcon(QIcon(":images/icone.png"));
	setWindowTitle(tr("client_irc: Favoris"));
	
	m_layoutPrincipal=new QVBoxLayout;
	m_favoris=new QTreeWidget;
	m_favoris->headerItem()->setText(0,tr("Nom"));
	m_favoris->headerItem()->setText(1,tr("AutoJoin"));
	m_favoris->setColumnCount(2);
	charger();
	m_layoutPrincipal->addWidget(m_favoris);
	m_layoutBouton=new QHBoxLayout;
	m_boutonEnregistrer=new QPushButton(tr("Enregistrer"));
	m_boutonEnregistrer->setAutoDefault(true);
	m_layoutBouton->addWidget(m_boutonEnregistrer);
	m_boutonConnecter=new QPushButton(tr("&Connecter"));
	m_layoutBouton->addWidget(m_boutonConnecter);
	m_boutonAjouterServeur=new QPushButton(tr("&Ajouter un serveur"));
	m_layoutBouton->addWidget(m_boutonAjouterServeur);
	m_boutonAjouterDiscussion=new QPushButton(tr("&Ajouter une discussion"));
	m_layoutBouton->addWidget(m_boutonAjouterDiscussion);
	m_boutonEditer=new QPushButton(tr("&Editer"));
	m_layoutBouton->addWidget(m_boutonEditer);
	m_boutonSupprimer=new QPushButton(tr("&Supprimer"));
	m_layoutBouton->addWidget(m_boutonSupprimer);
	m_boutonFermerSansEnregistrer=new QPushButton(tr("Fermer sans enregistrer"));
	m_layoutBouton->addWidget(m_boutonFermerSansEnregistrer);
	m_layoutPrincipal->addLayout(m_layoutBouton);
	setLayout(m_layoutPrincipal);
	show();
	
	connect(m_boutonConnecter,SIGNAL(clicked()),this,SLOT(connecter()));
	connect(m_boutonSupprimer,SIGNAL(clicked()),this,SLOT(supprimer()));
	connect(m_boutonAjouterServeur,SIGNAL(clicked()),this,SLOT(ajouterServeur()));
	connect(m_boutonAjouterDiscussion,SIGNAL(clicked()),this,SLOT(ajouterDiscussion()));
	connect(m_boutonEditer,SIGNAL(clicked()),this,SLOT(editer()));
	connect(m_boutonEnregistrer,SIGNAL(clicked()),this,SLOT(close()));
	connect(m_boutonFermerSansEnregistrer,SIGNAL(clicked()),this,SLOT(nePasEnregistrer()));
	
	m_fenEditionServeurFavoris=NULL;
	m_fenEditionDiscussionFavoris=NULL;
	m_enregistrerALaFermeture=true;
}

void FenFavoris::closeEvent(QCloseEvent * event)
{
	if(m_enregistrerALaFermeture) enregistrer();
	if(m_fenEditionServeurFavoris!=NULL) delete m_fenEditionServeurFavoris;
	if(m_fenEditionDiscussionFavoris!=NULL) delete m_fenEditionDiscussionFavoris;
}

void FenFavoris::nePasEnregistrer()
{
	m_enregistrerALaFermeture=false;
	close();
}

void FenFavoris::supprimer()
{
	supprimer(m_favoris->currentItem());
}

void FenFavoris::supprimer(QTreeWidgetItem * element)
{
	if(m_favoris->indexOfTopLevelItem(element)!=-1)	m_favoris->takeTopLevelItem(m_favoris->indexOfTopLevelItem(element));
	else element->parent()->removeChild(element);
	m_listeFavoris.removeOne(element);
}

void FenFavoris::connecter()
{
	QTreeWidgetItem * element=m_favoris->currentItem();
	if(m_favoris->indexOfTopLevelItem(element)!=-1)
	{
		emit connection(element->text(0));
	}
	else emit join(element->text(0),element->parent()->text(0));
}

void FenFavoris::ajouterServeur()
{
	QTreeWidgetItem * element=new QTreeWidgetItem(QStringList(tr("irc.serveur.net")));
	element->setCheckState(1,Qt::Unchecked);
	m_listeFavoris<<element;
	m_favoris->addTopLevelItem(element);
	m_fenEditionServeurFavoris=new FenEditionServeurFavoris(element,this);
	connect(m_fenEditionServeurFavoris,SIGNAL(termine(QTreeWidgetItem *,QString)),this,SLOT(ajouter_(QTreeWidgetItem *,QString)));
	connect(m_fenEditionServeurFavoris,SIGNAL(annule(QTreeWidgetItem *)),this,SLOT(annulerAjouter(QTreeWidgetItem *)));	
}

void FenFavoris::ajouterDiscussion()
{
	QTreeWidgetItem * element=m_favoris->currentItem();
	element=m_favoris->indexOfTopLevelItem(element)!=-1 ? element : element->parent();
	QTreeWidgetItem * discussion=new QTreeWidgetItem(QStringList(tr("#chan")));
	discussion->setCheckState(1,Qt::Unchecked);
	element->addChild(discussion);
	m_listeFavoris<<discussion;
	m_fenEditionDiscussionFavoris=new FenEditionDiscussionFavoris(discussion,this);
	connect(m_fenEditionDiscussionFavoris,SIGNAL(annule(QTreeWidgetItem *)),this,SLOT(annulerAjouter(QTreeWidgetItem *)));
}

void FenFavoris::annulerAjouter(QTreeWidgetItem * element)
{
	supprimer(element);
}

void FenFavoris::editer()
{
	QTreeWidgetItem * element=m_favoris->currentItem();
	if(m_favoris->indexOfTopLevelItem(element)!=-1)	m_fenEditionServeurFavoris=new FenEditionServeurFavoris(element,this);
	else m_fenEditionDiscussionFavoris=new FenEditionDiscussionFavoris(element,this);
}

void FenFavoris::enregistrer()
{
	QFile * fichierAutoJoin;
	fichierAutoJoin=new QFile(m_emplacementDonnees+"/conf/favoris.conf");
	fichierAutoJoin->open(QIODevice::WriteOnly|QIODevice::Text);
	QTextStream streamAutoJoin(fichierAutoJoin);
	QList<QTreeWidgetItem *>::iterator i,j;
	for(i=m_listeFavoris.begin();i!=m_listeFavoris.end();++i)
	{
		if(m_favoris->indexOfTopLevelItem(*i)!=-1)
		{
			streamAutoJoin<<((*i)->checkState(1)==Qt::Checked ? "+" : "")<<"@"<<(*i)->text(0)<<"\n";
			for(j=m_listeFavoris.begin();j!=m_listeFavoris.end();++j)
			{
				if((*j)->parent()==(*i)) streamAutoJoin<<((*j)->checkState(1)==Qt::Checked ? "+" : "")<<(*j)->text(0)<<"\n";
			}
		}
	}
	fichierAutoJoin->close();
}


void FenFavoris::charger()
{
	QFile * fichierAutoJoin;
	QString temp="";
	fichierAutoJoin=new QFile(m_emplacementDonnees+"/conf/favoris.conf");
	fichierAutoJoin->open(QIODevice::ReadOnly|QIODevice::Text);
	QTreeWidgetItem * serveurEnCours=NULL;
	QTreeWidgetItem * discussion=NULL;
	while(!fichierAutoJoin->atEnd())
	{
		temp=fichierAutoJoin->readLine();
		temp=temp.mid(0,temp.size()-1);
		if(temp.startsWith("@") || temp.startsWith("+@"))
		{
			serveurEnCours=new QTreeWidgetItem(QStringList(temp.startsWith("+") ? temp.mid(2) : temp.mid(1)));
			serveurEnCours->setCheckState(1,temp.startsWith("+") ? Qt::Checked : Qt::Unchecked);
			m_listeFavoris<<serveurEnCours;
			m_favoris->addTopLevelItem(serveurEnCours);
		}
		else if(serveurEnCours!=NULL)
		{
			discussion=new QTreeWidgetItem(QStringList(temp.startsWith("+") ? temp.mid(1) : temp));
			discussion->setCheckState(1,temp.startsWith("+") ? Qt::Checked : Qt::Unchecked);
			serveurEnCours->addChild(discussion);
			m_listeFavoris<<discussion;
		}
	}
	fichierAutoJoin->close();
}

FenEditionFavoris::FenEditionFavoris(QTreeWidgetItem * element,QWidget * parent) : QDialog(parent)
{
	setMinimumSize(500,300);
	setWindowIcon(QIcon(":images/icone.png"));
	m_element=element;
	m_nom=new QLineEdit(m_element->text(0));
	m_autoJoin=new QCheckBox(tr("AutoJoin"));
	m_autoJoin->setChecked(m_element->checkState(1)==Qt::Checked);
	m_layoutPrincipal=new QVBoxLayout;
	m_layoutFormulaire=new QFormLayout;
	m_layoutFormulaire->addRow(tr("Nom :"),m_nom);
	m_layoutFormulaire->addRow(m_autoJoin);
	m_layoutPrincipal->addLayout(m_layoutFormulaire);
	m_layoutBouton=new QHBoxLayout;
	m_boutonOk=new QPushButton(tr("Ok"));
	m_boutonOk->setAutoDefault(true);
	m_boutonAnnuler=new QPushButton(tr("Annuler"));
	m_layoutBouton->setAlignment(Qt::AlignRight);
	m_layoutBouton->addWidget(m_boutonOk);
	m_layoutBouton->addWidget(m_boutonAnnuler);
	m_layoutPrincipal->addLayout(m_layoutBouton);
	setLayout(m_layoutPrincipal);
	show();
	
	connect(m_boutonOk,SIGNAL(clicked()),this,SLOT(terminer()));
	connect(m_boutonAnnuler,SIGNAL(clicked()),this,SLOT(close()));
	m_termine=false;
}

void FenEditionFavoris::terminer()
{
	m_element->setText(0,m_nom->text());
	m_element->setCheckState(1,m_autoJoin->isChecked() ? Qt::Checked : Qt::Unchecked);
	emit termine();
	m_termine=true;
	close();
}

void FenEditionFavoris::closeEvent(QCloseEvent * event)
{
	if(!m_termine) emit annule(m_element);
}

FenEditionServeurFavoris::FenEditionServeurFavoris(QTreeWidgetItem * element,QWidget * parent) : FenEditionFavoris(element,parent)
{
	setWindowTitle(tr("client_irc: Edition de Favoris (serveur)"));
}

FenEditionDiscussionFavoris::FenEditionDiscussionFavoris(QTreeWidgetItem * element,QWidget * parent) : FenEditionFavoris(element,parent)
{
	
	setWindowTitle(tr("client_irc: Edition de Favoris (discussion)"));
}
