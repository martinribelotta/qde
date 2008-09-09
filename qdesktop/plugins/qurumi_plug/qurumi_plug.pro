TEMPLATE = lib

TARGET = qurumi_plug

HEADERS =
SOURCES = qurumi_plug.cpp

include(../qde_plugins.pri)

#target.path = $$QDEPREFIX/bin

INSTALLS += target
