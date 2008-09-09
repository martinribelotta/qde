TEMPLATE = lib

TARGET = dummy_plug
SOURCES = dummy_plug.cpp

#QDEPREFIX = $$(QDEPREFIX)
#isEmpty($$QDEPREFIX): QDEPREFIX="/opt/qde"

include(../qde_plugins.pri)

#target.path = $$QDEPREFIX/bin

INSTALLS += target
