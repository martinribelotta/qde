SOURCES       = main.cpp

## install
#PREFIX=$$(QDE_PREFIX)
#isEmpty($$PREFIX) {
#    PREFIX=/opt/qde
#}

include(qde.pri)

target.path = $$QDEPREFIX/bin
INSTALLS += target
