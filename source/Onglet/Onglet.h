#ifndef DEF_ONGLET
#define DEF_ONGLET
 
#include <QtGui>
#include <QtNetwork>
#include "EditionMemoire.h"

class ListeMessage : public QTextBrowser
{
	Q_OBJECT
	
	public:
	ListeMessage(QWidget * parent = 0);
};

class EditionCompletion : public EditionMemoire
{
	Q_OBJECT
	
	public:
	EditionCompletion();
	void ajouterMot(QString mot);
	
	private:
	bool event(QEvent * event);
	void completerMot();
	
	private:	
	QStringList m_listeMot;
};

class LabelPseudo : public QLabel
{
	Q_OBJECT
	
	public:
	LabelPseudo(const QString & text);
	
	private:
	void mousePressEvent(QMouseEvent * event);
	
	signals:
	void clicked();
};
enum TypeOnglet
{
	SERVEUR,
	CHAN,
	MP
};

class Onglet : public QWidget
{
	Q_OBJECT
	 
	public:
	Onglet(QString nom,QString css,QString emplacementDonnees);
	ListeMessage *lecture();
	EditionCompletion *edition();
	QFile *log();
	LabelPseudo * labelPseudo();
	QTreeWidgetItem *elementArbre();
	QString nom();
	TypeOnglet type();
	void setNom(QString nom);
	virtual void envoyer(QString texte)=0;
	void ecrire(QString texte);
	void ecrire(QString texte,QString cssClass);
	void ecrire(QString texte,QString cssClass,QString texteLog);
	void setOngletCourant(bool ongletCourant);
	QTextCodec * codec();
	void setCodec(QTextCodec * codec);
	void setCss(QString css);
	
	protected:
	QHBoxLayout * m_layoutDiscussionListe;
	QVBoxLayout * m_layoutLectureEdition;
	EditionCompletion * m_edition;
	ListeMessage * m_lecture;
	QTextDocument * m_document;
	QString m_nom;
	QFile *m_log;
	QTreeWidgetItem *m_elementArbre;
	TypeOnglet m_type;
	QHBoxLayout * m_layoutPseudoEdition;
	LabelPseudo * m_labelPseudo;
	bool m_ongletCourant;
	QTextCodec * m_codec;
	QString m_emplacementDonnees;
};
 
#endif
