#include "../interface.h"

#include <QtCore>
#include <QtGui>

#include <QtUiTools>
#include <QCopChannel>

static void rescueData( const QString& line, QString& name, QString& path ) {
	if ( line.startsWith( "/" ) ) {
		path = line;
		name = QFileInfo(line).baseName().replace("lib",QString()).replace(".so",QString());
	} else if ( line.startsWith( "*" ) ) {
		name = line.mid(1);
		path = QString("%1/../plugins/lib%2.so")
			.arg( QApplication::applicationDirPath() )
			.arg( name );
	} else {
		name = QFileInfo(line).baseName().replace("lib",QString()).replace(".so",QString());
		path = QString("%1/../plugins/%2")
			.arg( QApplication::applicationDirPath() )
			.arg( line );
	}
	path = QFileInfo( path ).canonicalFilePath();
}

class BottombarConfiguratorWidget: public QWidget {
	Q_OBJECT
public:
	BottombarConfiguratorWidget( QWidget *parent = 0l ): QWidget( parent ) {
		QUiLoader loader;
		QFile f( ":/formBar.ui" );
		if ( f.open( QFile::ReadOnly ) ) {
			QWidget *form = loader.load( &f, this );
			QHBoxLayout *layout = new QHBoxLayout( this );
			layout->addWidget( form );

			spinBox = qFindChild<QSpinBox*>( form, "spinBox" );
			pluginListView = qFindChild<QListWidget*>( form, "pluginListView" );

			QMetaObject::connectSlotsByName( this );

			loadConfig();
			loadPlugins();
		}
	}

	int barSize() { return spinBox->value(); }

public slots:
	void loadConfig() {
		QSettings sets( "qde", "desktop" );
		sets.beginGroup( "bottombar" );
		spinBox->setValue( sets.value( "height", 80 ).toInt() );
	}

	void loadPlugins() {
		QFile f( QSettings( "qde", "bottombar_plugins" ).fileName() );
		if ( f.open( QFile::ReadOnly ) ) {
			while ( !f.atEnd() ) {
				QString line = QString(f.readLine()).replace("\n",QString());
				if ( line.startsWith("#") || line.isEmpty() )
					continue;
				QString name, path;
				rescueData( line, name, path );
				QListWidgetItem *item = new QListWidgetItem( pluginListView );
				item->setText( name );
				item->setData( Qt::UserRole, path );
			}
		} else
			qDebug() << "Can't open" << f.fileName();
	}

	void saveConfig() {
		QSettings sets( "qde", "desktop" );
		sets.beginGroup( "bottombar" );
		sets.setValue( "height", barSize() );
	}

	void savePlugins() {
		// TODO
		QFile f( QSettings( "qde", "bottombar_plugins" ).fileName() );
		if ( f.open( QFile::WriteOnly ) ) {
			for( int i=0; i<pluginListView->count(); i++ ) {
				QListWidgetItem *item = pluginListView->item(i);
				QString name = item->text();
				QString path = item->data( Qt::UserRole ).toString();
				QString line = QString(
						"# %1\n"
						"%2\n\n"
					).arg(name).arg(path).toLocal8Bit();
				//qDebug() << line;
				f.write(line.toLocal8Bit());
			}
		} else
			qDebug() << "Can't open" << f.fileName();
	}

private slots:
	void moveCurrentItemTo( int delta ) {
		int row = pluginListView->currentRow();
		QListWidgetItem *item = pluginListView->takeItem( row );
		if ( item ) {
			row+=delta;
			if ( row<0 ) row=0;
			if ( row>pluginListView->count() ) row=pluginListView->count();
			pluginListView->insertItem( row, item );
			pluginListView->setCurrentItem( item );
		}
	}

	void on_buttonUpPlugin_clicked() {
		moveCurrentItemTo( -1 );
	}

	void on_buttonDownPlugin_clicked() {
		moveCurrentItemTo( 1 );
	}

	void on_buttonAddPlugin_clicked() {
		qDebug() << "on buttonAddPlugin clicked: TODO";
	}

	void on_buttonDelPlugin_clicked() {
		qDebug() << "on buttonDelPlugin clicked: TODO";
	}

	void on_buttonApply_clicked() {
		saveConfig();
		savePlugins();
		QCopChannel::send( "org.qde.qonfigurator.bottombar", "reconfigure" );
	}

	void on_buttonRestore_clicked() {
		qDebug() << "on buttonRestore clicked: TODO";
	}

private:
	QListWidget *pluginListView;
	QSpinBox *spinBox;
};

DECLARE_QONFIGURATOR_PLUGIN(
	"BottomBarConfigurator",
	QIcon( ":/bar.svg" ),
	QObject::tr( "Bottom Bar\nSettings" ),
	new BottombarConfiguratorWidget()
);

#include "main.moc"
