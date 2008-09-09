TARGET = qdesktop

QT += svg

include(../qde.pri)

HEADERS = bottombar.h desktop.h pluginloader.h
SOURCES = bottombar.cpp desktop.cpp main.cpp pluginloader.cpp

target.path = $$QDEPREFIX/bin
INSTALLS += target
