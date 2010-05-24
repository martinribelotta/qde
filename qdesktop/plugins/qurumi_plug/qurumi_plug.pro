TEMPLATE = lib

TARGET = qurumi_plug

HEADERS =
SOURCES = qurumi_plug.cpp
RESOURCES = qurumi_plug.qrc

include(../qde_plugins.pri)

#target.path = $$QDEPREFIX/bin

INSTALLS += target
