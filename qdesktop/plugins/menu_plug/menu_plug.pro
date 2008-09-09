TEMPLATE = lib

TARGET = menu_plug
SOURCES = menu_plug.cpp
RESOURCES = menu_plug.qrc

include(../qde_plugins.pri)

#target.path = $$QDEPREFIX/bin

INSTALLS += target
