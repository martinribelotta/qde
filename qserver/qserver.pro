HEADERS       = qserver.h qserver_p.h
SOURCES       = qserver.cpp main.cpp

#INCLUDEPATH  += . ./include

## install
#PREFIX=$$(QDE_PREFIX)
#isEmpty($$PREFIX) {
#    PREFIX=/opt/qde
#}

include(qde.pri)

target.path = $$QDEPREFIX/bin
INSTALLS += target
