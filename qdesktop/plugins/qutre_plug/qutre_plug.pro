TEMPLATE = lib

TARGET = qutre_plug

HEADERS =
SOURCES = qutre_plug.cpp

include(../qde_plugins.pri)

#target.path = $$QDEPREFIX/bin

INSTALLS += target
