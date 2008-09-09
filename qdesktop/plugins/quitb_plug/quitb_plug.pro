TEMPLATE = lib

TARGET = quitb_plug
SOURCES = quitb_plug.cpp
RESOURCES = quitb_plug.qrc

include(../qde_plugins.pri)

#target.path = $$QDEPREFIX/bin

INSTALLS += target
