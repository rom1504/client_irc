DESTDIR  = ../../bin/plugin/
TARGET = PluginIA
VPATH = ../../source/plugin
DEPENDPATH += .
INCLUDEPATH += $$VPATH
CONFIG(release, release|debug) {
	OBJECTS_DIR = ../../bin/temp/release/plugin/
} else {
	OBJECTS_DIR = ../../bin/temp/debug/plugin/
}

MOC_DIR = $$OBJECTS_DIR
RCC_DIR = $$OBJECTS_DIR
UI_DIR = $$OBJECTS_DIR
TEMPLATE=lib
CONFIG+=release plugin
HEADERS=PluginIA.h Plugin.h
SOURCES=PluginIA.cpp
QT+=network