TEMPLATE = lib

CONFIG += uitools plugin

HEADERS = ../interface.h
SOURCES = main.cpp
RESOURCES = resourceStyle.qrc

include(../qde.pri)

target.path = $$QDEPREFIX/plugins/qonfigurator

INSTALLS = target
