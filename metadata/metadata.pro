TEMPLATE = app
TARGET = qonfigurator
HEADERS = \
	qonfiguratorwidgets/configdialog.h \
	qonfiguratorwidgets/faderwidget.h \
	../qonfiguratorplug/interface.h

SOURCES = main.cpp \
	qonfiguratorwidgets/configdialog.cpp \
	qonfiguratorwidgets/faderwidget.cpp

include(qde.pri)

config.files = \
	settings/desktop.conf \
	settings/desktop_plugins.conf \
	settings/qdeserver.rc \
	settings/qurumi_menu.conf \
	settings/.qderc \
	settings/bottombar_plugins.conf

config.path = $$QDEPREFIX/Settings/qde

share.files = share/*
share.path = $$QDEPREFIX/share/

target.path = $$QDEPREFIX/bin

INSTALLS += share config target
