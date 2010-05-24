TARGET = qdesktop

QT += svg

include(../qde.pri)

HEADERS = bottombar.h desktop.h pluginloader.h desktop_item.h
SOURCES = bottombar.cpp desktop.cpp main.cpp pluginloader.cpp desktop_item.cpp

target.path = $$QDEPREFIX/bin
INSTALLS += target
