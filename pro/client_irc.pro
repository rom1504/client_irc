TEMPLATE = app
DESTDIR  = ../bin/
TARGET = client_irc
VPATH = ../source/Fenetre ../source/Onglet ../source/ ../source/Plugin
DEPENDPATH += .
INCLUDEPATH += $$VPATH
CONFIG(release, release|debug) {
	OBJECTS_DIR = ../bin/temp/release/
} else {
	OBJECTS_DIR = ../bin/temp/debug/
}

MOC_DIR = $$OBJECTS_DIR
RCC_DIR = $$OBJECTS_DIR
UI_DIR = $$OBJECTS_DIR

HEADERS += FenPrincipale.h Onglet.h OngletServeur.h OngletDiscussion.h OngletChan.h OngletMp.h FenConnexion.h FenPreference.h FenAPropos.h FenFavoris.h EditionMemoire.h Rang.h Plugin.h FenPlugin.h
SOURCES += FenPrincipale.cpp FenPrincipaleEnvoyer.cpp FenPrincipaleRecevoir.cpp main.cpp Onglet.cpp OngletServeur.cpp OngletDiscussion.cpp OngletChan.cpp OngletMp.cpp FenConnexion.cpp FenPreference.cpp FenAPropos.cpp FenFavoris.cpp EditionMemoire.cpp Rang.cpp FenPlugin.cpp
RC_FILE += ../ressource/ressource.rc
RESOURCES += ../ressource/ressource.qrc
QT+=network
CONFIG+=release
TRANSLATIONS = ../langue/client_irc_en.ts