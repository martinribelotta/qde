#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QLibrary>
#include <QGridLayout>

#include <QtSvg>

#include "desktop.h"
#include "../plugins/include/plugins.h"

QDEsktop::QDEsktop(): QWidget( QApplication::desktop() ) {
	layout = new QGridLayout( this );
	setFixedSize( QApplication::desktop()->width(),
		QApplication::desktop()->height() );
	setWindowFlags( Qt::FramelessWindowHint );

	setAttribute( Qt::WA_NoSystemBackground );
	setAttribute( Qt::WA_OpaquePaintEvent );
	loadBackground();
	loadMoqoids();
}

void QDEsktop::showEvent( QShowEvent* /*e*/ ) {
	lower();
}

void QDEsktop::paintEvent( QPaintEvent *e ) {
	QPainter p( this );
	foreach( QRect r, e->region().rects() )
		p.drawPixmap( r.topLeft(), background, r );
}

/**
 * It is a mokooooo!!!! It's roto mal!!!!
 */
void QDEsktop::loadBackground() {
	QSettings sets( "qde", "desktop" );
	sets.beginGroup( "background" );
	QColor backgroundColor( sets.value("fillcolor","#000000").toString() );
	QString backgroundFile( sets.value( "filename", "" ).toString() );
	QRect deskRect( 0, 0,
		QApplication::desktop()->width(),
		QApplication::desktop()->height() );
	background= QPixmap( deskRect.size() );
	background.fill( backgroundColor );
	if ( !backgroundFile.isEmpty() ) {
		QPainter p( &background );
		if ( QFileInfo( backgroundFile ).suffix().toLower() == ".svg" )
			QSvgRenderer( backgroundFile ).render( &p, deskRect );
		else
			p.drawPixmap( deskRect, QPixmap( backgroundFile ) );
	}
}

static void msg( const QString& key, const QString& text ) {
	qDebug() << "loadMoqoids FATAL [" << key << "]: " << text;
}

void QDEsktop::loadMoqoids() {
	QSettings sets( "qde", "desktop" );
	sets.beginGroup( "Moqoids" );
	foreach( QString key, sets.childGroups() ) {
		QRect r(
			sets.value( key+"/x", 10 ).toInt(),
			sets.value( key+"/y", 10 ).toInt(),
			sets.value( key+"/w", 320 ).toInt(),
			sets.value( key+"/h", 200 ).toInt()
		);
		QString pluginPath = sets.value( key+"/path", QString() ).toString();
		if ( !pluginPath.isEmpty() ){
			MoqoidPluginLoader_t loader = (MoqoidPluginLoader_t)
				QLibrary::resolve( pluginPath, MOQOID_ENTRY_NAME );
			if ( loader ) {
				QWidget *w = loader( this );
				if ( w ) {
					//w->setParent( this );
					w->move( r.topLeft() );
					w->setFixedSize( r.size() );
					w->show();
				} else
					msg( key, "Entry point function return null" );
			} else
				msg( key, QString( MOQOID_ENTRY_NAME " not found on library %1")
					.arg( pluginPath ) );
		} else
			msg( key, "plugin path key not found" );
	}
}
