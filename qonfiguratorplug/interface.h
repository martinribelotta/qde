#ifndef QDE_QONFIGURE_PLUGIN_INTERFACE_H
#define QDE_QONFIGURE_PLUGIN_INTERFACE_H

#include <QString>
#include <QIcon>

struct QOnfiguratorPlugin_data {
	QString name;
	QIcon icon;
	QString description;
	QWidget *widget;
	QOnfiguratorPlugin_data(
		const QString& _name,
		const QIcon& _icon,
		const QString& _desc,
		QWidget *w ):
			name(_name),
			icon(_icon),
			description(_desc),
			widget(w) { }
};

typedef QOnfiguratorPlugin_data (*QOnfiguratorPlugin_entry)();

#define QONFIGURATOR_PLUGIN_ENTRY_NAME "qonfigurator_plugin_entry"

#define DECLARE_QONFIGURATOR_PLUGIN(name,icon,description,widgetclass) \
	extern "C" QOnfiguratorPlugin_data qonfigurator_plugin_entry() { \
		return QOnfiguratorPlugin_data(name,icon,description,widgetclass); \
	}

#endif /* QDE_QONFIGURE_PLUGIN_INTERFACE_H */
