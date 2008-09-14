TEMPLATE = lib

CONFIG += uitools

HEADERS = ../interface.h
SOURCES = main.cpp
RESOURCES = resourceBar.qrc

include(../qde.pri)

target.path = $$QDEPREFIX/plugins/qonfigurator

INSTALLS = target
