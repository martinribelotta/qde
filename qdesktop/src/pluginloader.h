#ifndef QDE_PLUGIN_LOADER_H
#define QDE_PLUGIN_LOADER_H

#include <QObject>

class QDEPluginLoader_private;

class QDEPluginLoader: public QObject {
public:
	QDEPluginLoader();
	~QDEPluginLoader();
	void loadAll();
private:
	QDEPluginLoader_private *self;
};

#endif /* QDE_PLUGIN_LOADER_H */
