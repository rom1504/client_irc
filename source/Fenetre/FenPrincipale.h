#ifndef DEF_FENPRINCIPALE
#define DEF_FENPRINCIPALE
 
#include <QtGui>
#include <QtNetwork>
#include "Onglet.h"
#include "OngletServeur.h"
#include "OngletDiscussion.h"
#include "OngletChan.h"
#include "OngletMp.h"
#include "FenConnexion.h"
#include "FenPreference.h"
#include "FenFavoris.h"
#include "FenAPropos.h"
#include "FenPlugin.h"
#include "Rang.h"
#include "Plugin.h"

class FenPrincipale : public QMainWindow
{
	Q_OBJECT
	 
	public:
	FenPrincipale();//constructeur
	
	private://menus de la fenetre
	QMenu * m_menuGeneral;
	QMenu * m_menuServeur;
	QMenu * m_menuOutils;
	QMenu * m_menuAide;
	QMenu * m_menuPlugin;
	
	private://système tray
	QMenu * m_menuSysTray;
	QIcon *m_iconeSysTray;
	QIcon *m_iconeSysTrayNouveauMessage;
	QSystemTrayIcon *m_sysTray;
	QTimer *m_timerClignoter;
	
	private://actions
	QAction *m_actionFermer;
	QAction *m_actionCacher;
	QAction *m_actionQuitter;
	QAction *m_actionConnecter;
	QAction *m_actionReconnecter;
	QAction *m_actionDeconnecter;
	QAction *m_actionPreference;
	QAction *m_actionFavoris;
	QAction *m_actionPlugin;
	QAction *m_actionAPropos;
	QAction *m_actionAide;
	
	private://parametres
	QMap<QString,QString> m_parametres;	
	
	private://redéfinition
	bool event(QEvent * e);
	void closeEvent(QCloseEvent *e);
	
	private://fonctions recevoir
	void recevoir(QByteArray ligne,OngletServeur * serveur);
	void recevoirPrivmsg(QString recu,OngletServeur * serveur);
	void recevoirNotice(QString recu,OngletServeur *serveur);
	void recevoirPing(QString recu,OngletServeur * serveur);
	void recevoirJoin(QString recu,OngletServeur * serveur);
	void recevoirPart(QString recu,OngletServeur * serveur);
	void recevoirMode(QString recu,OngletServeur * serveur);
	void recevoirNick(QString recu,OngletServeur * serveur);
	void recevoirQuit(QString recu,OngletServeur * serveur);
	void recevoirTopic(QString recu,OngletServeur * serveur);
	void recevoirCommandeNumeriqueSansReaction(QString recu,OngletServeur * serveur);
	void recevoir001(QString recu,OngletServeur * serveur);
	void recevoir002(QString recu,OngletServeur * serveur);
	void recevoir305(QString recu,OngletServeur * serveur);
	void recevoir306(QString recu,OngletServeur * serveur);
	void recevoir315(QString recu,OngletServeur * serveur);
	void recevoir317(QString recu,OngletServeur * serveur);
	void recevoir332(QString recu,OngletServeur * serveur);
	void recevoir333(QString recu,OngletServeur * serveur);
	void recevoir352(QString recu,OngletServeur * serveur);
	void recevoir353(QString recu,OngletServeur * serveur);
	void recevoir474(QString recu,OngletServeur * serveur);
	void recevoirKick(QString recu,OngletServeur * serveur);
	void recevoirInvite(QString recu,OngletServeur * serveur);
	void recevoirPong(QString recu,OngletServeur * serveur);
	QMap<QString,void (FenPrincipale::*)(QString,OngletServeur *)> m_recevoirCommandes;
	QStringList m_commandesSansReaction;
	
	private://fonction envoyer
	void envoyerClientHelp(QString parametre,Onglet * onglet);
	void envoyerSet(QString parametre,Onglet * onglet);
	void envoyerTopic(QString parametre,Onglet * onglet);
	void envoyerNick(QString parametre,Onglet * onglet);
	void envoyerQuit(QString parametre);
	void envoyerQuit(QString parametre,Onglet * onglet);
	void envoyerMe(QString parametre,Onglet * onglet);
	void envoyerPrivmsg(QString parametre,Onglet * onglet);
	void envoyerPart(QString parametre,Onglet * onglet);
	void envoyerCycle(QString parametre,Onglet * onglet);
	void envoyerKick(QString parametre,Onglet * onglet);
	void envoyerCharset(QString parametre,Onglet * onglet);
	void envoyerQuote(QString parametre,Onglet * onglet);
	void envoyerReconnect(QString parametre,Onglet * onglet);
	void envoyerMOp(QString parametre,Onglet * onglet);
	void envoyerMKick(QString parametre,Onglet * onglet);
	void envoyerInvite(QString parametre,Onglet * onglet);
	void envoyerClose(QString parametre,Onglet * onglet);
	void envoyerQuery(QString parametre,Onglet * onglet);
	void envoyerMode(QString parametre,Onglet * onglet);
	void envoyerOp(QString parametre,Onglet * onglet);
	void envoyerDeOp(QString parametre,Onglet * onglet);
	void envoyerVoice(QString parametre,Onglet * onglet);
	void envoyerDeVoice(QString parametre,Onglet * onglet);
	void envoyerBan(QString parametre,Onglet * onglet);
	void envoyerDeBan(QString parametre,Onglet * onglet);
	void envoyerKickBan(QString parametre,Onglet * onglet);
	void envoyerMsg(QString parametre,Onglet * onglet);
	void envoyerVersion(QString parametre,Onglet * onglet);
	void envoyerWho(QString parametre,Onglet * onglet);
	void envoyerAway(QString parametre,Onglet * onglet);
	void envoyerBack(QString parametre,Onglet * onglet);
	void envoyerPing(QString parametre,Onglet * onglet);
	QMap<QString,void (FenPrincipale::*)(QString,Onglet *)> m_envoyerCommandes;
	QMap<QString,QString> m_aideCommandes;
	
	private slots:
	void fermer();
	void cacher();
	void ouvrirAide();
	void ouvrirAPropos();
	void ouvrirPreference();
	void ouvrirFavoris();
	void ouvrirPlugin();
	void connecter_(QString serveur,QString pseudo,int port);
	void connecter(QString serveur);
	void connecterFenetre();
	void envoyer();
	void recevoir();
	void recevoir_();
	void deconnecte();
	void changementElement(QTreeWidgetItem *element);
	void envoyerQuit();
	void reduire(QSystemTrayIcon::ActivationReason reason);
	void clignoter();
	void suiteAutoJoin(OngletServeur * serveur);
	void supprimerLorsqueDeconnecte();
	void envoyerNick();
	void envoyerMp();
	void envoyerTopic();
	void quitter();
	void rejoindre(QString discussion,QString serveur);
	void envoyerMp(QString parametre,Onglet * onglet);
	void envoyerJoin(QString parametre,Onglet * onglet);
	void reconnecter();
	void mettreAJourOngletCss();
	void traitementMessageReponsePlugin(QString emetteur,QString serveur,QString message);
	
	signals:
	void connecte(OngletServeur * serveur);
	
	private:// fonctions en lien avec les plugins
	void traitementMessagePlugin(QString pseudo,QString message,Onglet * onglet);	
	
	private:// fonctions diverses
	void connecter(QString serveur,QString pseudo,int port,bool ouvertAvecAutoJoin=false);
	OngletDiscussion * discussion(QString nom,OngletServeur * serveur);
	OngletServeur * trouverServeur(QString nom);
	Onglet * ongletElementArbre(QTreeWidgetItem *element);
	Onglet * m_ongletCourant;
	void autoJoin();
	void changerOnglet(Onglet * onglet);
	void mettreAJourTitreFenetre();
	QString ajouterPersonne(OngletChan * chan,QString pseudo);
	void enleverPersonne(OngletChan * chan,QString pseudo);
	void quitterMp(OngletMp * mp);
	void rejoindreLesChansOuverts(OngletServeur * serveur);
	void genererOngletCss();
	OngletServeur * ongletServeur(Onglet * onglet);
	void ajouterElementDiscussionArbre(OngletDiscussion * discussion);
	
	private://variable diverses
	Rang m_rang;
	FenConnexion * m_fenConnexion;
	FenPreference * m_fenPreference;
	FenFavoris * m_fenFavoris;
	FenAPropos * m_fenAPropos;
	FenPlugin * m_fenPlugin;
	bool m_ongletOuvert;
	bool m_contructionTermine;
	bool m_clignotant;
	QWidget * m_zoneCentrale;
	QHBoxLayout *m_layoutPrincipal;
	QTreeWidget *m_arbre;
	bool m_fermer;
	QString m_ongletCss;
	QString m_emplacementDonnees;
	
	QIcon *m_iconeFenetre;
	QIcon *m_iconeOwner;
	QIcon *m_iconeBot;
	QIcon *m_iconeOp;
	QIcon *m_iconeHalfOp;
	QIcon *m_iconeVoice;
	QList <Onglet *> m_onglet;
	QList <Plugin *> m_plugin;
};
 
#endif
