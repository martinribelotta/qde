#ifndef QDESKTOP_PLUGIN_H
#define QDESKTOP_PLUGIN_H

#include <QHash>
#include <QString>
#include <QVariant>

typedef QWidget* (*QuiquePluginLoader_t)(void);
typedef QObject* (*QDesktopPluginLoader_t)(const QHash<QString,QVariant> & /*parameters*/);
typedef QWidget* (*MoqoidPluginLoader_t)( QWidget *parent );

#define QUIQUE_ENTRY_NAME "quique_plugin_loader"
#define QDESKTOP_ENTRY_NAME "qdesktop_plugin_loader"
#define MOQOID_ENTRY_NAME "moqoid_plugin_loader"

/**
 * quique_plugin_loader is an entry point to the plugin.
 * This function is responsible to initialize the widget and
 * return it or NULL if fail.
 */
#define DECLARE_QUIQUE_CREATOR() \
	extern "C" QWidget *quique_plugin_loader()

/**
 * qdesktop_plugin_loader is an entry point to the general QDesktop plugin.
 * The function is responsible to create a QObject plugin and return
 * it or NULL if fail.
 * QHash<QString,QVariant> parameters are the parameter passed to the
 * plugin in <key,value> hash table. At least, it include "name" and "plugin"
 * keys with name and full path of plugin, respectively
 */
#define DECLARE_QDESKTOP_CREATOR() \
	extern "C" QObject *qdesktop_plugin_loader( const QHash<QString,QVariant> & parameters )

/**
 * moqoid_plugin_loader is an entry point to make a moqoid widget for desktop
 * geom is a geometry specified for configuration file or automatic grid align.
 */
#define DECLARE_MOQOID_CREATOR() \
	extern "C" QWidget *moqoid_plugin_loader( QWidget *parent )

#endif /* QDESKTOP_PLUGIN_H */
