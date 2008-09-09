TEMPLATE = app
TARGET = qonfigurator
SOURCES = main.cpp

include(qde.pri)

config.files = \
	settings/desktop.conf \
	settings/desktop_plugins.conf \
	settings/qdeserver.rc \
	settings/qurumi_menu.conf \
	settings/.qderc
config.path = $$QDEPREFIX/Settings/qde

share.files = share/*
share.path = $$QDEPREFIX/share/
INSTALLS += share config
