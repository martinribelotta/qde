#include <QSettings>
#include <QLibrary>
#include <QList>
#include <QStringList>
#include <QHash>
#include <QString>
#include <QVariant>
#include <QApplication>

#include <QtDebug>

#include "../plugins/include/plugins.h"
#include "pluginloader.h"

struct PluginInfo {
	QHash<QString,QVariant> parameters;
	QObject *plugin;
};

class QDEPluginLoader_private {
public:
	QList<PluginInfo> pluginList;
	QSettings sets;
	
	QDEPluginLoader_private():
		sets( "qde", "desktop_plugins" )
	{
	}
};

static inline void msg( const QString& pluginName, const QString& msg ) {
	qDebug() << "FATAL on \""  << pluginName << "\": " << msg;
}

QDEPluginLoader::QDEPluginLoader()
{
	self = new QDEPluginLoader_private;
}

QDEPluginLoader::~QDEPluginLoader()
{
	foreach( PluginInfo info, self->pluginList )
		delete info.plugin;
	delete self;
}

void QDEPluginLoader::loadAll()
{
	foreach( QString pluginName, self->sets.childGroups() ) {
		self->sets.beginGroup( pluginName );
		PluginInfo info;
		info.parameters["name"] = QVariant(pluginName);
		foreach( QString key, self->sets.childKeys() )
			info.parameters[ key ] = self->sets.value( key );
		if ( info.parameters.contains( "plugin" ) ) {
			QString pluginPath = info.parameters[ "plugin" ].toString();
			QDesktopPluginLoader_t entryPoint =
				(QDesktopPluginLoader_t)QLibrary::resolve(
					pluginPath, QDESKTOP_ENTRY_NAME );
			if ( entryPoint ) {
				QObject *pluginObject = entryPoint( info.parameters );
				if ( pluginObject ) {
					info.plugin = pluginObject;
					pluginObject->setParent( QApplication::instance() );
					self->pluginList.append( info );
					qDebug() << "pluginLoader: " << pluginName << " loaded";
				} else
					msg(pluginName,"Entry point fail on construct plugin");
			} else
				msg(pluginName,"Can't find " QDESKTOP_ENTRY_NAME " into plugin file");
		} else
			msg(pluginName,"Can't find 'plugin' entry");
	}
}
