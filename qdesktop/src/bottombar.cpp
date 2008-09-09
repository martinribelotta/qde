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

#include <QtDebug>

#include "../plugins/include/plugins.h"

static void sendMaxWindowRect( const QRect& r ) {
	QByteArray data;
	QDataStream stream( &data, QIODevice::WriteOnly );
	stream << r;
	QCopChannel::send( "org.qde.server", "setMaxWindowRect", data );
}

static QWidget *loadPlugin( const QString& /*title*/, const QString& path ) {
	QString realPath( path );
	if ( path.startsWith('/') )
		realPath = path;
	else if ( path.startsWith('*') )
		realPath = QApplication::applicationDirPath()+"/"+path;

	QuiquePluginLoader_t loader = (QuiquePluginLoader_t)QLibrary::resolve( realPath, QUIQUE_ENTRY_NAME );
	if ( loader )
		return loader();
	else
		qDebug() << "FATAL: not enougth " QUIQUE_ENTRY_NAME " on " << realPath << " It`s a quique plugin?";
	return 0l;
}

BottomBar::BottomBar() {
	setWindowFlags( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );
	QSettings sets( "qde", "desktop" );

	sets.beginGroup( "bottombar" );

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing( sets.value( "space", 1 ).toInt() );

	QRect r = QApplication::desktop()->geometry();
	setFixedSize( r.width(), sets.value( "height", 80 ).toInt() );
	r.setBottom( r.bottom()-height() );
	sendMaxWindowRect( r );
	move( r.bottomLeft()+QPoint(0,1) );

	sets.endGroup();
	sets.beginGroup( "plugins" );

	QStringList keys = sets.childKeys();
	keys.sort();
	foreach( QString title, keys ) {
		// Format is:
		//   title=/absolute/path/to/plugin.so
		// if start with '/', if start with '*', the path is relative to qdesktop binary
		// In other case not try to determine path and use LD_LIBRARY_PATH etc.
		QString key = sets.value( title ).toString();
		QWidget *w = loadPlugin( title, key );
		if ( w ) {
			w->setParent( this );
			layout->addWidget(w);
			qDebug() << title << " as been loaded from " << key;
		} else
			qDebug() << "FATAL: Can't create plugin. Loader fail returning null";
	}
	sets.endGroup();

}

BottomBar::~BottomBar() {
}

void BottomBar::paintEvent( QPaintEvent * /*e*/ ) {
	static bool initGradient = true;
	static QLinearGradient grad( 0,0,0,height() );
	if ( initGradient ) {
		QColor light = palette().color( QPalette::Button );
		QColor dark = palette().color( QPalette::Dark );
		grad.setColorAt( 0.0, light );
		grad.setColorAt( 0.5, dark );
		//grad.setColorAt( 0.5, light.darker( 180 ) );
		grad.setColorAt( 1.0, light );
		initGradient = false;
	}
	QPainter p( this );
	p.fillRect( QRect(0,0,width(),height()), QBrush( grad ) );
}
