#include <QtGui>
#include "FenPrincipale.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QString locale=QLocale::system().name().section('_',0,0);
	QTranslator translator,translatorDialog;
	QDir dossierConf(QCoreApplication::applicationDirPath());
	dossierConf.cdUp();
	dossierConf.cd("langue");
	translator.load(QString(dossierConf.absolutePath()+"/client_irc_")+locale);
	translatorDialog.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&translator);
	app.installTranslator(&translatorDialog);
	Q_INIT_RESOURCE(ressource);
	FenPrincipale fenetre;

	return app.exec();
}
