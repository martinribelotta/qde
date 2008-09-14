TEMPLATE = lib

CONFIG += uitools

HEADERS = ../interface.h
SOURCES = main.cpp
RESOURCES = resourceWallpaper.qrc

include(../qde.pri)

target.path = $$QDEPREFIX/plugins/qonfigurator

INSTALLS = target
