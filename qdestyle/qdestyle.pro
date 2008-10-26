TEMPLATE = lib
CONFIG += plugin

SOURCES = \
	qdestyle.cpp \
	qdeproxystyle.cpp \
	qdeclientmanager.cpp

HEADERS = \
	qdestyle.h \
	qdeproxystyle.h \
	qdeclientmanager.h

target.path = $$[QT_INSTALL_PLUGINS]/styles
INSTALLS += target
