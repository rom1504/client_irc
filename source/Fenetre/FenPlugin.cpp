#include "FenPlugin.h"

FenPlugin::FenPlugin(QString emplacementDonnees) : QDialog(),m_emplacementDonnees(emplacementDonnees)
{
	setModal(true);
	setMinimumSize(700,400);
	setWindowIcon(QIcon(":images/icone.png"));
	setWindowTitle(tr("client_irc: Plugin"));
	
	m_layoutPrincipal=new QVBoxLayout;
	m_listePlugin=new QTableWidget;
	m_listePlugin->setColumnCount(2);
	m_listePlugin->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Nom")));
	m_listePlugin->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("A charger")));
	m_listePlugin->verticalHeader()->hide();
	m_listePlugin->setSortingEnabled(false);
	charger();
	m_layoutPrincipal->addWidget(m_listePlugin);
	setLayout(m_layoutPrincipal);
	show();
}

void FenPlugin::closeEvent(QCloseEvent * event)
{
	enregistrer();
}

void FenPlugin::enregistrer()
{
	QFile * fichierPlugin;
	fichierPlugin=new QFile(m_emplacementDonnees+"/conf/plugin.conf");
	fichierPlugin->open(QIODevice::WriteOnly|QIODevice::Text);
	QTextStream streamPlugin(fichierPlugin);
	for(int i=0;i<m_listePlugin->rowCount();i++) streamPlugin<<(m_listePlugin->item(i,1)->checkState()==Qt::Checked ? "+" : "-")<<m_listePlugin->item(i,0)->text()<<"\n";
	fichierPlugin->close();
}


void FenPlugin::charger()
{
	QDir pluginsDir=QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugin");
	QStringList extensionPlugin;
	extensionPlugin<<"*.dll"<<"*.so"<<"*.dylib";
	QStringList plugins=pluginsDir.entryList(extensionPlugin,QDir::Files);
	m_listePlugin->setRowCount(plugins.count());
	int i=0;
	QTableWidgetItem * caseACocher;
	foreach(QString plugin,plugins)
	{
		m_listePlugin->setItem(i,0,new QTableWidgetItem(plugin));
		m_listePlugin->setItem(i,1,caseACocher=new QTableWidgetItem);
		caseACocher->setCheckState(Qt::Unchecked);
		i++;
	}
	
	QFile * fichierPlugin;	
	QString temp="";
	fichierPlugin=new QFile(m_emplacementDonnees+"/conf/plugin.conf");
	fichierPlugin->open(QIODevice::ReadOnly|QIODevice::Text);
	QList<QTableWidgetItem*> listeTemporaire;
	while(!fichierPlugin->atEnd())
	{
		temp=fichierPlugin->readLine();
		temp=temp.mid(0,temp.size()-1);
		listeTemporaire=m_listePlugin->findItems(temp.mid(1),Qt::MatchExactly);
		if(listeTemporaire.size()>0) m_listePlugin->item(listeTemporaire.first()->row(),1)->setCheckState(temp.startsWith("+") ? Qt::Checked : Qt::Unchecked);
	}
	fichierPlugin->close();
}
