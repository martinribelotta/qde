#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QLibrary>
#include <QtSvg>
#include <QCopChannel>

#include "desktop.h"
#include "../plugins/include/plugins.h"

#include "desktop_item.h"

QDEsktop::QDEsktop(): QWidget( QApplication::desktop() ) {
    setFixedSize( QApplication::desktop()->width(),
        QApplication::desktop()->height() );
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
    if ( QSettings( "qde", "desktop" ).value( "mocoidable", false ).toBool() )
        loadMoqoids();
    else
        loadFileIcons();
}

void QDEsktop::showEvent( QShowEvent* /*e*/ ) {
	lower();
}

void QDEsktop::mousePressEvent( QMouseEvent* e ) {
    (void)e;
    QPoint p = mapToGlobal( e->pos() );
    QByteArray data;
    QDataStream stream( &data, QIODevice::WriteOnly );
    stream << p;
    QCopChannel::send( "org.qde.plugins.qurumi", "showAt", data );
}

void QDEsktop::paintEvent( QPaintEvent* e ) {
	QPainter p( this );
	p.setRenderHints(QPainter::Antialiasing, true);
	p.setRenderHints(QPainter::SmoothPixmapTransform, true);
	p.setRenderHints(QPainter::TextAntialiasing, true);
	foreach( QRect r, e->region().rects() )
		p.drawPixmap( r.topLeft(), background, r );
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
	background = QPixmap( deskRect.size() );
	background.fill( backgroundColor );
	if ( !backgroundFile.isEmpty() ) {
		QPainter p( &background );
		p.setRenderHints(QPainter::Antialiasing, true);
		p.setRenderHints(QPainter::SmoothPixmapTransform, true);
		if ( QFileInfo( backgroundFile ).suffix().toLower() == ".svg" )
			QSvgRenderer( backgroundFile ).render( &p, deskRect );
		else
			p.drawPixmap( deskRect, QPixmap( backgroundFile ) );
	}
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
					w->setParent( this );
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

void QDEsktop::loadFileIcons() {
    QSettings sets( "qde", "desktop" );
    QStringList deskPath = sets.value( "iconsPath", QDir::homePath()+"/Desktop" ).toString().split(':');
    foreach( QDEDesktopItem* i, findChildren<QDEDesktopItem*>() ) {
        delete i;
    }

    //QList<QDEDesktopItem*> itemList;
    QPoint p( 10, 10 );
    int maxw = 0, maxh = 0;
    foreach( QString s, deskPath ) {
        QDir dir( s, "*.qdesk" );
        foreach( QFileInfo fi, dir.entryInfoList( QDir::Files ) ) {
            QDEDesktopItem *item = new QDEDesktopItem( fi.absoluteFilePath(), this );
            connect( item, SIGNAL(movedTo(QDEDesktopItem*,const QPoint&)),
                     this, SLOT(adjustItem(QDEDesktopItem*,const QPoint&)) );
            itemList.append( item );
            if ( item->width()>maxw )
                maxw = item->width();
            if ( item->height()>maxh )
                maxh = item->height();
        }
    }
    gridX = maxw+4;
    gridY = maxh+4;
    QRect r( 2, 2, maxw+2, maxh+2 );
    foreach( QDEDesktopItem *item, itemList ) {
        item->setGeometry( r );
        r.translate( 0, r.height() );
        if ( !QApplication::desktop()->availableGeometry().contains( r, true ) ) {
            r.moveTopLeft( QPoint( 2, r.right() ) );
        }
    }
}

void QDEsktop::adjustItem( QDEDesktopItem* item, const QPoint& p ) {
    QPoint adjusted = QPoint( 2+(p.x()/gridX)*gridX, 2+(p.y()/gridY)*gridY );
    item->move( adjusted );
}
