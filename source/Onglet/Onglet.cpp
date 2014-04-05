#include "Onglet.h"

ListeMessage::ListeMessage(QWidget * parent) : QTextBrowser(parent)
{
	
}

EditionCompletion::EditionCompletion() : EditionMemoire()
{
	
}

bool EditionCompletion::event(QEvent * event)
{
	if(event->type()==QEvent::KeyPress && ((QKeyEvent *)event)->key()==Qt::Key_Tab) completerMot();
	else QLineEdit::event(event);
	return true;
}

void EditionCompletion::completerMot()
{
	QString mot=text().left(cursorPosition()).section(" ",-1,-1);
	m_listeMot.sort();
	for(int i=0;i<m_listeMot.size();i++)
	{
		if(m_listeMot.at(i).startsWith(mot,Qt::CaseInsensitive))
		{
			cursorWordBackward(true);
			cut();
			insert(m_listeMot.at(i)+" ");
			break;
		}
	}
}

void EditionCompletion::ajouterMot(QString mot)
{
	m_listeMot<<mot;
}

LabelPseudo::LabelPseudo(const QString & text) : QLabel(text)
{
	
}

void LabelPseudo::mousePressEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton) emit clicked();
}

Onglet::Onglet(QString nom,QString css,QString emplacementDonnees) : QWidget(),m_emplacementDonnees(emplacementDonnees),m_nom(nom)
{
	m_codec=QTextCodec::codecForName("UTF-8");
	m_elementArbre=new QTreeWidgetItem(QStringList(nom));
	m_edition=new EditionCompletion;
	m_lecture=new ListeMessage;
	m_lecture->setReadOnly(true);
	m_document=new QTextDocument;
	m_lecture->setDocument(m_document);
	setCss(css);
	m_lecture->setOpenExternalLinks(true);
	m_lecture->verticalScrollBar()->setValue(m_lecture->verticalScrollBar()->maximum());	
	m_lecture->setStyleSheet("ListeMessage{background-color:#FFF;font-size:9pt;font-family:Verdane;}");
	m_layoutLectureEdition=new QVBoxLayout;
	m_layoutLectureEdition->addWidget(m_lecture);
	m_layoutPseudoEdition=new QHBoxLayout;
	m_layoutPseudoEdition->addWidget(m_edition);
	m_layoutLectureEdition->addLayout(m_layoutPseudoEdition);
	m_layoutDiscussionListe=new QHBoxLayout;
	m_layoutDiscussionListe->addLayout(m_layoutLectureEdition,8);
	setLayout(m_layoutDiscussionListe);
	m_ongletCourant=false;
}


void Onglet::setOngletCourant(bool ongletCourant)
{
	m_ongletCourant=ongletCourant;
}

QTextCodec * Onglet::codec()
{
	return m_codec;
}

void Onglet::setCodec(QTextCodec * codec)
{
	m_codec=codec;
}

void Onglet::setCss(QString css)
{
	m_document->setDefaultStyleSheet("a{text-decoration:none;}"+css);
}

QString Onglet::nom()
{
	return m_nom;
}

void Onglet::setNom(QString nom)
{
	m_nom=nom;
}

LabelPseudo * Onglet::labelPseudo()
{
	return m_labelPseudo;
}

ListeMessage *Onglet::lecture()
{
	return m_lecture;
}
void Onglet::ecrire(QString texte)
{
	ecrire(texte,"noir");
}
void Onglet::ecrire(QString texte,QString cssClass)
{
	ecrire(texte,cssClass,"");
}
void Onglet::ecrire(QString texte,QString cssClass,QString texteLog)
{
	//log
	QTextStream streamLog(m_log);
	texteLog=(texteLog!="" ? texteLog : texte);
	texteLog.replace("&lt;","<").replace("&gt;",">");
	streamLog<<QDate::currentDate().toString("d/M/yyyy")+" "+QTime::currentTime().toString("[hh:mm]")+" "+texteLog+"\n";
	
	//remplacement texte
	texte.replace("<","&lt;").replace(">","&gt;");
	QRegExp regexColor("\\[color=(.+)\\](.+)\\[/color\\]");
	regexColor.setMinimal(true);
	texte.replace(regexColor,"<span style=\"color:\\1;\">\\2</span>");
	/*QRegExp regexCouleur("\003([0-9]+)(.+)");
	regexCouleur.setMinimal(true);
	texte.replace(regexCouleur,"<span style=\"color:blue;\">\\2</span>");*/
	texte.replace(QRegExp("([a-zA-Z\\./]+\\.[a-zA-Z]{2,3})"),"<a href=\"http://\\1\">\\1</a>");
	
	//position de la barre de defilement
	bool descendu=m_lecture->verticalScrollBar()->value()==m_lecture->verticalScrollBar()->maximum();
	int positionBarre=m_lecture->verticalScrollBar()->value();
	QTextCursor curseur=m_lecture->textCursor();
	curseur.movePosition(QTextCursor::End);
	
	
	
	curseur.insertHtml("<div class=\""+cssClass+"\"><span style=\"color:"+(new QColor(150,150,150))->name()+"\">"+QTime::currentTime().toString("[hh:mm]")+"</span> "+texte+"<br /></div>");
	
	//position de la barre de defilement
	if(descendu) m_lecture->verticalScrollBar()->setValue(m_lecture->verticalScrollBar()->maximum());
	else m_lecture->verticalScrollBar()->setValue(positionBarre);
	
	//notification du nouveau texte
	if(!m_ongletCourant) m_elementArbre->setForeground(0,QBrush(QColor(120,0,0)));
}
EditionCompletion *Onglet::edition()
{
	return m_edition;
}

QFile *Onglet::log()
{
	return m_log;
}

QTreeWidgetItem *Onglet::elementArbre()
{
	return m_elementArbre;
}

TypeOnglet Onglet::type()
{
	return m_type;
}
