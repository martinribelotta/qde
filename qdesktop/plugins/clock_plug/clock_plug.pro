TEMPLATE = lib

TARGET = clock_plug
SOURCES = clock_plug.cpp

include(../qde_plugins.pri)

#target.path = $$QDEPREFIX/lib

INSTALLS += target
