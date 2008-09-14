/**
 * Background confioguration plugin
 * copyright (C) 2008 by Martin Ribelotta
 */

#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>

#include <QtCore>
#include <QtGui>
#include <QtUiTools>

#include <QCopChannel>

#include "../interface.h"


#define KNOW_FILES_TYPES "*.svg|*.png|*.jpg|*.jpeg|*.bmp"

static QImage scaleImage( const QString& filename, const QSize& z ) {
	return QImage( filename ).scaled( z, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}

class BackgroundConfiguratorWidget: public QWidget {
	Q_OBJECT
public:
	BackgroundConfiguratorWidget( QWidget *parent = 0l ): QWidget( parent ) {
		QUiLoader loader;
		QFile f( ":/formWallpaper.ui" );
		if ( f.open( QFile::ReadOnly ) ) {
			QWidget *form = loader.load( &f, this );
			QHBoxLayout *layout = new QHBoxLayout( this );
			layout->addWidget( form );

			wallpaperList = qFindChild<QListWidget*>( form, "wallpaperList" );
			labelWallpaperPreview = qFindChild<QLabel*>( form, "labelWallpaperPreview" );
			buttonSetFillColor = qFindChild<QToolButton*>( form, "buttonSetFillColor" );

			scalerWatcher = new QFutureWatcher<QImage>( this );
			connect(scalerWatcher, SIGNAL(finished()),
				this, SLOT(scalerWatcher_finished()) );

			QMetaObject::connectSlotsByName( this );

			loadWallpapers();
			loadConfiguration();

		}
	}

public slots:
	void loadWallpapers() {
		wallpaperList->clear();
		QDir dir( QApplication::applicationDirPath()+"/../share/wallpapers" );
		QStringList knowFileTypes = QString( KNOW_FILES_TYPES ).split("|");
		foreach( QFileInfo fileInfo, dir.entryInfoList( knowFileTypes, QDir::Files ) )
			addWallpaperFile( fileInfo );
	}

	void loadConfiguration() {
		QSettings sets( "qde", "desktop" );
		sets.beginGroup( "background" );
		QString fileName = sets.value( "filename" ).toString();
		QColor c( sets.value( "fillcolor", "#000000" ).toString() );
		wallpaperList->setCurrentItem( addWallpaperFile( QFileInfo( fileName ) ) );
		setFillColor( c );
	}

private:
	QListWidget *wallpaperList;
	QLabel *labelWallpaperPreview;
	QToolButton *buttonSetFillColor;
	QFutureWatcher<QImage> *scalerWatcher;
	QPixmap previewPixmap;

protected:
	QString getWallpaperFilename() {
		QListWidgetItem *item = wallpaperList->currentItem();
		return item? item->data( Qt::UserRole ).toString():QString();
	}

	QListWidgetItem *addWallpaperFile( const QFileInfo& fileInfo ) {
		QList<QListWidgetItem*> it = wallpaperList->findItems( fileInfo.baseName(), Qt::MatchExactly );
		if ( it.count() == 0 ) {
			QListWidgetItem *item = new QListWidgetItem( fileInfo.baseName(), wallpaperList );
			item->setData( Qt::UserRole, fileInfo.absoluteFilePath() );
			return item;
		}
		return it.at(0);
	}

	QColor getFillColor() {
		return buttonSetFillColor->palette().color( QPalette::Button );
	}

	void setFillColor( const QColor& c ) {
		QPalette pal( buttonSetFillColor->palette() );
		pal.setColor( QPalette::Button, c );
		buttonSetFillColor->setPalette( pal );
		updatePreview();
	}

private slots:
	void scalerWatcher_finished() {
		QApplication::setOverrideCursor( QCursor( Qt::ArrowCursor ) );
		previewPixmap = QPixmap::fromImage( scalerWatcher->result() );
		updatePreview();
	}

	void updatePreview() {
		QPixmap pix( labelWallpaperPreview->size() );
		pix.fill( getFillColor() );
		if ( !previewPixmap.isNull() ) {
			QPainter p( &pix );
			p.drawPixmap( QPoint(0,0), previewPixmap ); 
			p.end();
		}
		labelWallpaperPreview->setPixmap( pix );
	}

	void on_wallpaperList_currentItemChanged( QListWidgetItem* curr, QListWidgetItem* /*prev*/ ) {
		QString fileName = curr->data( Qt::UserRole ).toString();
		QSize size = labelWallpaperPreview->size();
		QFuture<QImage> future = QtConcurrent::run( scaleImage, fileName, size );
		scalerWatcher->setFuture( future );
		QApplication::setOverrideCursor( QCursor( Qt::BusyCursor ) );
	}

	void on_buttonAddNewWallpaper_clicked() {
		QString fileName = QFileDialog::getOpenFileName( this,
			tr( "Select a wallpaper" ), QString(),
			tr( "Image files (%1);;All files (*)" )
				.arg( QString(KNOW_FILES_TYPES).replace('|',' ') ) );
		if ( !fileName.isEmpty() )
			wallpaperList->setCurrentItem( addWallpaperFile( QFileInfo( fileName ) ) );
	}
	void on_buttonApply_clicked() {
		QSettings sets( "qde", "desktop" );
		sets.beginGroup( "background" );
		sets.setValue( "filename", getWallpaperFilename() );
		sets.setValue( "fillcolor", getFillColor().name() );
		QCopChannel::send( "org.qde.qonfigurator.desktop", "reconfigure" );
	}
	void on_buttonRevert_clicked() {
		// TODO
		qDebug() << "on buttonRevert clicked";
	}
	void on_buttonSetFillColor_clicked() {
		QColor c( QColorDialog::getColor( getFillColor(), this ) );
		if ( c.isValid() )
			setFillColor( c );
	}
};

DECLARE_QONFIGURATOR_PLUGIN(
	/* Plugin name */	"BackgroundConfigurator",
	/* Icon */		QIcon( ":/desktop.svg" ),
	/* Description */	QObject::tr( "Background\nConfigurator" ),
	/* Widget */		new BackgroundConfiguratorWidget()
);

#include "main.moc"
