#include <QWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QDataStream>

#include <QWSServer>
#include <QCopChannel>

#include <QtDebug>

#include "../include/plugins.h"

static QRect adjustToDesktop( const QRect& r ) {
	QRect desk = QApplication::desktop()->availableGeometry();
	QRect ret = r;
	if ( ret.top()<desk.top() )
		ret.moveTop( desk.top() );
	if ( ret.left()<desk.left() )
		ret.moveLeft( desk.left() );
	if ( ret.bottom()>desk.bottom() )
		ret.moveBottom( desk.bottom() );
	if ( ret.right()>desk.right() )
		ret.moveRight( desk.right() );
	return ret.normalized();
}

class WinListWidget: public QWidget {
public:
	WinListWidget( QToolButton *b ) {
		QHBoxLayout *l = new QHBoxLayout( this );
		l->setSpacing( 0 );
		l->setContentsMargins( 0,0,0,0 );
		l->addWidget( winList = new QListWidget( this ) );

		menuButton = b;
		setWindowFlags( Qt::FramelessWindowHint|Qt::Popup );
		resize( sizeHint() );
		//setFixedSize( 240, 300 );
	}
	void showAt( const QPoint& p ) {
		move( p );
		setGeometry( adjustToDesktop( geometry() ) );
		show();
	}
	inline QListWidget *windowsList() { return winList; }
protected:
	virtual void hideEvent( QHideEvent* ) {
		menuButton->setChecked( false );
	}
	virtual void showEvent( QShowEvent* ) {
		menuButton->setChecked( true );
	}
private:
	QToolButton *menuButton;
	QListWidget *winList;
};

class WinListButton: public QToolButton {
	Q_OBJECT
public:
	WinListButton(): winListWidget(this) {
		setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
		setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		setArrowType( Qt::UpArrow );
		//setIcon( QPixmap( ":/top.png" ) );
		setText( tr("Window List") );
		winListWidget.hide();
		connect(winListWidget.windowsList(), SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(clickItem(QListWidgetItem*)));
		setCheckable( true );
		connect(this, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
		connect(new QCopChannel( "org.qde.server.wm" ), SIGNAL(received(const QString&,const QByteArray&)),
			this, SLOT(windowEvent(const QString&,const QByteArray&)));
	}
private slots:
	void onClick(bool) {
		winListWidget.showAt( mapToGlobal( pos() ) );
	}
	void clickItem( QListWidgetItem* item ) {
		//TODO
		int wid = item->data( Qt::UserRole ).toInt();
		QByteArray data;
		QDataStream stream( &data, QIODevice::WriteOnly );
		stream << wid;
		QCopChannel::send( "org.qde.server", "wm:setActiveWindow", data );
		winListWidget.hide();
	}

	void delWindow( int wid ) {
		if ( windowsMap.contains( wid ) ) {
			QListWidgetItem *item = windowsMap[ wid ];
			windowsMap.take( wid );
			delete item;
			winListWidget.resize( winListWidget.sizeHint() );
		}
	}

	void addWindow( int wid, const QString& caption ) {
		QListWidgetItem *item;
		if ( windowsMap.contains( wid ) )
			item = windowsMap[ wid ];
		else {
			item = new QListWidgetItem( winListWidget.windowsList() );
			windowsMap[ wid ] = item;
		}
		item->setText( caption );
		item->setData( Qt::UserRole, wid );
		winListWidget.windowsList()->setCurrentItem( item );
		winListWidget.resize( winListWidget.sizeHint() );
	}

	void windowEvent( const QString& message, const QByteArray& data ) {
		if ( message == "windowEvent" ) {
			quint32 wid, ev, flags;
			QString caption;
			QDataStream stream( data );
			stream >> wid >> ev >> flags >> caption;
			int windowType = flags&Qt::WindowType_Mask;
			switch ( (QWSServer::WindowEvent)ev ) {
				case QWSServer::Create:
				case QWSServer::Active:
				case QWSServer::Raise:
				case QWSServer::Show:
				case QWSServer::Geometry:
				case QWSServer::Name:
					if ( !caption.isEmpty() )
						addWindow( wid, caption );
				break;
				case QWSServer::Destroy:
					delWindow( wid );
				break;
				case QWSServer::Hide:
				case QWSServer::Lower:
					// Empty
				break;
			}
		}
	}
private:
	QHash<int,QListWidgetItem*> windowsMap;
	WinListWidget winListWidget;
};

DECLARE_QUIQUE_CREATOR() {
	return new WinListButton();
}

#include "winlist_plug.moc"
