#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QLibrary>

#include <QtSvg>

#include <QCopChannel>

#ifdef QDESKTOP_EXPERIMENTAL

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

#warning QDESKTOP_EXPERIMENTAL is ***EXPERIMENTAL*** (look it?)

#endif

#include "desktop.h"
#include "../plugins/include/plugins.h"

QDEsktop::QDEsktop():
#ifdef QDESKTOP_EXPERIMENTAL
    QGraphicsView
#else
    QWidget
#endif
    ( QApplication::desktop() ) {
	setFixedSize( QApplication::desktop()->width(),
		QApplication::desktop()->height() );
#ifdef QDESKTOP_EXPERIMENTAL
	setScene( new QGraphicsScene( this ) );
	scene()->setSceneRect( viewport()->geometry() );
#endif
	setWindowFlags( Qt::FramelessWindowHint );

	connect(
		new QCopChannel( "org.qde.qonfigurator.desktop" ),
		SIGNAL(received(const QString&,const QByteArray&)),
		this,
		SLOT(reconfigure(const QString&,const QByteArray&))
	);

	setAttribute( Qt::WA_NoSystemBackground );
	setAttribute( Qt::WA_OpaquePaintEvent );
	loadBackground();
	loadMoqoids();
}

void QDEsktop::showEvent( QShowEvent* /*e*/ ) {
	lower();
}

void QDEsktop::paintEvent( QPaintEvent* e ) {
#ifdef QDESKTOP_EXPERIMENTAL
	QGraphicsView::paintEvent( e );
#else
	QPainter p( this );
	foreach( QRect r, e->region().rects() )
		p.drawPixmap( r.topLeft(), background, r );
#endif
}

void QDEsktop::reconfigure( const QString& /*message*/, const QByteArray& /*data*/ ) {
	loadBackground();
	update();
}

void QDEsktop::loadBackground() {
	QSettings sets( "qde", "desktop" );
	sets.beginGroup( "background" );
	QColor backgroundColor( sets.value("fillcolor","#000000").toString() );
	QString backgroundFile( sets.value( "filename", "" ).toString() );
	QRect deskRect( 0, 0,
		QApplication::desktop()->width(),
		QApplication::desktop()->height() );
#ifdef QDESKTOP_EXPERIMENTAL
	QPixmap
#endif
		background = QPixmap( deskRect.size() );
	background.fill( backgroundColor );
	if ( !backgroundFile.isEmpty() ) {
		QPainter p( &background );
		p.setRenderHints(
			QPainter::Antialiasing|
			QPainter::SmoothPixmapTransform|
			QPainter::TextAntialiasing,
			true
		);
		if ( QFileInfo( backgroundFile ).suffix().toLower() == ".svg" )
			QSvgRenderer( backgroundFile ).render( &p, deskRect );
		else
			p.drawPixmap( deskRect, QPixmap( backgroundFile ) );
	}
#ifdef QDESKTOP_EXPERIMENTAL
	setBackgroundBrush( background );
#endif
}

static void msg( const QString& key, const QString& text ) {
	qDebug() << "loadMoqoids FATAL [" << key << "]: " << text;
}

/**
 * It is a mokooooo!!!! It's roto mal!!!!
 */
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
				QWidget *w = loader( 0l );
				if ( w ) {
#ifdef QDESKTOP_EXPERIMENTAL
					QDialog *d = new QDialog;
					d->setWindowTitle( key );
					QHBoxLayout *l = new QHBoxLayout( d );
					w->setParent( d );
					l->addWidget( w );
					scene()->addWidget( d, Qt::Window );
					d->show();
#else
					w->setParent( this );
					w->move( r.topLeft() );
					w->setFixedSize( r.size() );
					w->show();
#endif
				} else
					msg( key, "Entry point function return null" );
			} else
				msg( key, QString( MOQOID_ENTRY_NAME " not found on library %1")
					.arg( pluginPath ) );
		} else
			msg( key, "plugin path key not found" );
	}
}
