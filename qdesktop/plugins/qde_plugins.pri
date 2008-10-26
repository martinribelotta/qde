#QDEPREFIX = $$(QDEPREFIX)
#isEmpty($$QDEPREFIX): QDEPREFIX="/opt/qde"

include(../qde.pri)
QDEPREFIX_PLUGINS = $$QDEPREFIX/plugins
target.path = $$QDEPREFIX_PLUGINS

CONFIG += plugin
