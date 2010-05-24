#include <QWidget>
#include <QCopChannel>
#include <QDataStream>
#include <QRect>
#include <QIODevice>
#include <QSettings>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QCache>
#include <QMenu>

#include <QtDebug>

#include "../include/plugins.h"

#define QDE_BASEDIR QString(QApplication::applicationDirPath()+"/../")
#define DEFAULT_META_ICON QDE_BASEDIR+"/share/icons/filesystems/exec.png"
#define DEFAULT_META_TEXT "..."
#define DEFAULT_META_EXEC "/bin/true"
#define DEFAULT_META_TOOLTIP "Unknow"
#define DEFAULT_CATEGORY_ICON QDE_BASEDIR+"/share/icons/filesystems/folder.png"

class Qurumi: public QObject {
	Q_OBJECT
public:
	Qurumi() {
		QCopChannel *channel = new QCopChannel( "org.qde.plugins.qurumi", this );
		connect(channel, SIGNAL(received(const QString&,const QByteArray&)),
			this, SLOT(eventMenu(const QString&,const QByteArray&)) );

		favoritesSubMenu = new QMenu( tr("Favorites") );

		loadConfiguration();
		connect( &menu, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)) );

		menu.addSeparator();
		menu.addMenu( favoritesSubMenu );
		menu.addSeparator();
		menu.addAction( QIcon(QDE_BASEDIR+"/share/icons/actions/system-shutdown.png"), tr("Shutdown"), this, SLOT(halt()) );
		menu.addAction( QIcon(QDE_BASEDIR+"/share/icons/actions/system-restart.png"), tr("Restart"), this, SLOT(restart()) );
		menu.addAction( QIcon(QDE_BASEDIR+"/share/icons/actions/system-log-out.png"), tr("Logout"), this, SLOT(logout()) );
	}

protected:
	QAction *createItem( const QSettings& qdesk ) {
		QString text = qdesk.value( "text", DEFAULT_META_TEXT ).toString();
		QIcon icon( qdesk.value( "icon", DEFAULT_META_ICON ).toString() );
		QString exec = qdesk.value( "exec", DEFAULT_META_EXEC ).toString();
		QAction *item = new QAction( icon, text, &menu );
		item->setData( exec );
		return item;
	}

	QMenu *findOrNewCategory( const QString& categoryName ) {
		QMenu *subMenu;
		if ( categories.contains( categoryName ) )
			subMenu = categories[ categoryName ];
		else {
			subMenu = menu.addMenu( categoryName );
			categories[ categoryName ] = subMenu;
		}
		return subMenu;
	}

public slots:
	void loadConfiguration() {
		QStringList qdeskPath = QSettings( "qde", "qurumi_menu" )
			.value( "qdeskpath", QDE_BASEDIR+"/share/menu" ).toString().split(":");
		foreach( QString path, qdeskPath ) {
			QDir dir( path );
			foreach( QFileInfo entry, dir.entryInfoList( QStringList("*.qdesk"), QDir::Files ) )
				loadConfigurationFrom( entry.absoluteFilePath() );
		}
	}

	void loadConfigurationFrom( const QString& filename ) {
		QSettings qdesk( filename, QSettings::IniFormat );
		QStringList categoryNameList = qdesk.value( "category", "unknow" ).toString().split(":");
		foreach( QString categoryName, categoryNameList ) {
			QAction *item = createItem( qdesk );
			if ( categoryName == "favorites" ) {
				favoritesSubMenu->addAction( item );
			} else {
				QMenu *category = findOrNewCategory( categoryName );
				category->addAction( item );
			}
		}
	}

private slots:
	void eventMenu( const QString& message, const QByteArray& data ) {
		QDataStream stream(data);
		if ( message == "showRect" ) {
			QRect r;
			stream >> r;
			menu.setGeometry( r );
			menu.exec( r.topLeft() );
		} else if ( message == "showAt" ) {
			QPoint pos;
			stream >> pos;
			menu.exec( pos );
		} else if ( message == "show" ) {
			menu.exec();
		} else if ( message == "hide" ) {
			menu.hide();
		}
	}

	void triggered( QAction *action ) {
		if ( action ) {
			QString executable = action->data().toString();
			if ( !executable.isEmpty() ) {
				qDebug() << "Launching " << executable;
				QByteArray data;
				QDataStream stream( &data, QIODevice::WriteOnly );
				stream << executable;
				//QCopChannel::send( "org.qde.qtroll", "exec", data );
				QProcess::startDetached( executable );
			}
		}
	}

	void logout() {
		QCopChannel::send( "org.qde.server", "quit" );
	}

	void restart() {
		QCopChannel::send( "org.qde.qdm.system", "restart" );
	}

	void halt() {
		QCopChannel::send( "org.qde.qdm.system", "halt" );
	}

private:
	QMenu menu;
	QMenu *favoritesSubMenu;
	QHash<const QString&,QMenu*> categories;
};

DECLARE_QDESKTOP_CREATOR() {
	(void)parameters; // Supress 'unused parameter...' warning
	return new Qurumi();
}

#include "qutre_plug.moc"
