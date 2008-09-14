#include "bottombar.h"

#include <QPainter>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#include <QDataStream>
#include <QCopChannel>

#include <QLinearGradient>
#include <QBrush>
#include <QHBoxLayout>
#include <QLibrary>
#include <QFile>

#include <QtDebug>

#include "../plugins/include/plugins.h"

#define DEFAULT_PLUGINS_PATH (QApplication::applicationDirPath()+"/../plugins/")

static void sendMaxWindowRect( const QRect& r ) {
	QByteArray data;
	QDataStream stream( &data, QIODevice::WriteOnly );
	stream << r;
	QCopChannel::send( "org.qde.server", "setMaxWindowRect", data );
}

static QWidget *loadPlugin( const QString& path ) {
	QString realPath( path );
	if ( path.startsWith('/') )
		realPath = path;
	else if ( path.startsWith('*') )
		realPath = DEFAULT_PLUGINS_PATH+QString("lib%1.so").arg(path.mid(1));
	else
		realPath = DEFAULT_PLUGINS_PATH+path;

	QuiquePluginLoader_t loader = (QuiquePluginLoader_t)QLibrary::resolve( realPath, QUIQUE_ENTRY_NAME );
	if ( loader )
		return loader();
	else
		qDebug() << "FATAL: not enougth " QUIQUE_ENTRY_NAME " on " << realPath << " It`s a quique plugin?";
	return 0l;
}

BottomBar::BottomBar() {
	setWindowFlags( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );

	layout = new QHBoxLayout(this);

	connect(new QCopChannel("org.qde.qonfigurator.bottombar"),
		SIGNAL(received(const QString&,const QByteArray&)),
		this,
		SLOT(reconfigure(const QString&,const QByteArray&))
	);

	loadConfig();
	loadPlugins();
}

BottomBar::~BottomBar() {
}

void BottomBar::paintEvent( QPaintEvent* ) {
	QLinearGradient grad( 0,0,0,height() );
	QColor light = palette().color( QPalette::Button );
	QColor dark = palette().color( QPalette::Dark );
	grad.setColorAt( 0.0, light );
	grad.setColorAt( 0.5, dark );
	grad.setColorAt( 1.0, light );
	QPainter p( this );
	p.fillRect( QRect(0,0,width(),height()), QBrush( grad ) );
}

void BottomBar::clearPlugins() {
	foreach( QWidget *w, pluginWidgets )
		delete w;
	pluginWidgets.clear();
}

void BottomBar::loadPlugins() {
	qDebug() << "Loading plugins...";
	QFile f( QSettings( "qde", "bottombar_plugins" ).fileName() );
	if ( f.open( QFile::ReadOnly ) ) {
		while ( !f.atEnd() ) {
			QString line = QString(f.readLine()).replace( "\n", QString() );
			if ( line.startsWith("#") || line.isEmpty() )
				continue;
			QWidget *w = loadPlugin( line );
			if ( w ) {
				w->setParent( this );
				layout->addWidget(w);
				pluginWidgets.append( w );
				qDebug() << "Plugin " << line << " as been loaded";
			} else
				qDebug() << "FATAL: Can't create " << line << " plugin. Loader fail returning null.";
		}
	} else
		qDebug() << "Can't open " << f.fileName();
	qDebug() << "done.";
}

void BottomBar::loadConfig() {
	qDebug() << "Loading configuration...";
	QSettings sets( "qde", "desktop" );

	sets.beginGroup( "bottombar" );

	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing( sets.value( "space", 1 ).toInt() );

	QRect r = QApplication::desktop()->geometry();
	setFixedSize( r.width(), sets.value( "height", 80 ).toInt() );
	r.setBottom( r.bottom()-height() );
	sendMaxWindowRect( r );
	move( r.bottomLeft()+QPoint(0,1) );

	sets.endGroup();
	qDebug() << "done.";
}

void BottomBar::reconfigure( const QString& message, const QByteArray& ) {
	if ( message == "reconfigure" ) {
		loadConfig();
		clearPlugins();
		loadPlugins();
	}
}
