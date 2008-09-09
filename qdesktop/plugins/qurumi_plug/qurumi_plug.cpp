#include <QWidget>
#include <QCopChannel>
#include <QDataStream>
#include <QRect>
#include <QIODevice>
#include <QPainter>
#include <QSettings>

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QColumnView>
#include <QComboBox>
#include <QSpacerItem>
#include <QToolButton>

#include <QListView>
#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>

#include <QListWidget>
#include <QListWidgetItem>

#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <QtDebug>

#include "../include/plugins.h"

//#define EXEC_ON_DCLICK

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

class QDeskMetaInfo {
	QSettings sets;
#define DEFAULT_META_ICON "/opt/qde/share/icons/mimetypes/unknown.png"
#define DEFAULT_META_TEXT "..."
#define DEFAULT_META_EXEC "/bin/true"
#define DEFAULT_META_TOOLTIP "Unknow"
public:
	QDeskMetaInfo( const QString& filename ):
		sets( filename, QSettings::IniFormat ) { sets.beginGroup( "Menu" ); }
	QIcon icon() {
		return QIcon( sets.value( "icon", DEFAULT_META_ICON ).toString() );
	}
	QString text() {
		return sets.value( "text", DEFAULT_META_TEXT ).toString();
	}
	QString exec() {
		return sets.value( "exec", DEFAULT_META_EXEC ).toString();
	}
	QString tooltip() {
		return sets.value( "tooltip", DEFAULT_META_TOOLTIP ).toString();
	}
	QVariant value( const QString& key, const QVariant& defaultValue ) {
		return sets.value( key, defaultValue );
	}
};

class QurumiMenu: public QWidget {
	Q_OBJECT
public:
	QurumiMenu() {
		setWindowFlags( Qt::Popup );
		makeGui();
		resize( sizeHint() );
		loadConfiguration();
	}
	~QurumiMenu() {
	}
protected:
	void paintEvent( QPaintEvent* ) {
		QPainter p( this );
		p.setPen( Qt::black );
		p.drawRect( QRect(0,0,width()-1,height()-1) );
	}
	void makeGui() {
		QVBoxLayout *vLayout1;
		QHBoxLayout *hLayout0;
		QHBoxLayout *hLayout1;
		QHBoxLayout *hLayout2;
		QToolButton *launchButton;
		QToolButton *buttonRestart;
		QToolButton *buttonShutdown;
		QToolButton *buttonLogout;
		QTabWidget *tab;
		resize(400, 300);

		vLayout1 = new QVBoxLayout(this);

		hLayout0 = new QHBoxLayout();
		launchEditor = new QComboBox(this);
		launchEditor->setEditable(true);
		hLayout0->addWidget(launchEditor);

		launchButton = new QToolButton(this);
		hLayout0->addWidget(launchButton);

		vLayout1->addLayout(hLayout0);

		QWidget *appWidget = new QWidget( this );
		categoryViewer = new QListWidget( appWidget ); //this);
		connect(categoryViewer, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			this, SLOT(categoryChange(QListWidgetItem*,QListWidgetItem*)) );
		itemsViewer = new QListWidget( appWidget ); //this);
#ifdef EXEC_ON_DCLICK
		connect(itemsViewer, SIGNAL(itemActivated(QListWidgetItem*)),
			this, SLOT(executeItem(QListWidgetItem*)) );
#else
		connect(itemsViewer, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(executeItem(QListWidgetItem*)) );
#endif
		hLayout2 = new QHBoxLayout( appWidget );
		hLayout2->setContentsMargins( 0,0,0,0 );
		hLayout2->addWidget(categoryViewer);
		hLayout2->addWidget(itemsViewer);

		favoritesViewer = new QListWidget( this );
		connect(favoritesViewer, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(executeItem(QListWidgetItem*)) );

		tab = new QTabWidget( this );
		tab->setIconSize( QSize(32,32) );
		tab->addTab( favoritesViewer, QIcon( "/opt/qde/share/icons/misc.png" ), tr( "Favorites" ) );
		tab->addTab( appWidget, QIcon( "/opt/qde/share/icons/tools.png" ), tr( "Applications" ) );

		tab->setTabPosition( QTabWidget::South );

		vLayout1->addWidget( tab );

		hLayout1 = new QHBoxLayout();

		buttonLogout = new QToolButton(this);
		hLayout1->addWidget(buttonLogout);
		buttonRestart = new QToolButton(this);
		hLayout1->addWidget(buttonRestart);
		buttonShutdown = new QToolButton(this);
		hLayout1->addWidget(buttonShutdown);

		hLayout1->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

		vLayout1->addLayout(hLayout1);

		launchButton->setText(tr( "Launch" ));
		
		buttonRestart->setText(tr( "Restart" ));
		buttonRestart->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		buttonRestart->setIcon(QIcon("/opt/qde/share/icons/restart.png"));
		buttonRestart->setIconSize( QSize(32,32) );

		buttonShutdown->setText(tr( "Shutdown" ));
		buttonShutdown->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		buttonShutdown->setIcon(QIcon("/opt/qde/share/icons/shutdown.png"));
		buttonShutdown->setIconSize( QSize(32,32) );

		buttonLogout->setText(tr( "Logout" ));
		buttonLogout->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		buttonLogout->setIcon(QIcon("/opt/qde/share/icons/stop.png"));
		buttonLogout->setIconSize( QSize(32,32) );

		connect(buttonRestart,SIGNAL(clicked()),
			this, SLOT(restart()) );
		connect(buttonShutdown,SIGNAL(clicked()),
			this, SLOT(halt()) );
		connect(buttonLogout,SIGNAL(clicked()),
			this, SLOT(logout()) );
		connect(launchButton,SIGNAL(clicked()),
			this, SLOT(executeCommand()) );
	}

	QListWidgetItem *makeItem( const QString& item, QSettings& sets ) {
		sets.beginGroup( item );
		QListWidgetItem *newItem = new QListWidgetItem(
			QIcon( sets.value( "icon", DEFAULT_META_ICON ).toString() ),
			sets.value( "text", DEFAULT_META_TEXT ).toString()
		);
		newItem->setData( Qt::UserRole, 
			sets.value( "exec", DEFAULT_META_EXEC ).toString() );
		sets.endGroup();
		return newItem;
	}

	QList<QListWidgetItem*> getCategoryItems( const QString& category, QSettings& sets ) {
		QList<QListWidgetItem*> currentCategoryList;
		sets.beginGroup( category );
		foreach( QString item, sets.childGroups() )
			currentCategoryList.append( makeItem( item, sets ) );
		sets.endGroup();
		return currentCategoryList;
	}

	void loadConfiguration() {
		QSettings sets( "qde", "qurumi_menu" );
		menuItemsList.clear();
		int rowId = 0;
		foreach( QString category, sets.childGroups() ) {
			QList<QListWidgetItem*> itemList = getCategoryItems( category, sets );
			if ( category == "favorites" ) {
				favoritesViewer->clear();
				foreach( QListWidgetItem *item, itemList )
					favoritesViewer->addItem( item );
			} else {
				QListWidgetItem *newCategory = new QListWidgetItem( categoryViewer );
				newCategory->setIcon( QIcon( "/opt/qde/share/icons/filesystems/folder.png" ) );
				newCategory->setText( category );
				newCategory->setData( Qt::UserRole, rowId++ );
				menuItemsList.append( itemList );
			}
		}
	}
private slots:
	void categoryChange( QListWidgetItem* curr, QListWidgetItem* /*prev*/ ) {
		int row = curr->data( Qt::UserRole ).toInt();
		const QList<QListWidgetItem*> &itemList = menuItemsList.at(row);
		while ( itemsViewer->count() )
			itemsViewer->takeItem(0);
		foreach( QListWidgetItem* it, itemList )
			itemsViewer->addItem( it );
	}
	void executeItem( QListWidgetItem* item ) {
		QString executable = item->data( Qt::UserRole ).toString();
		qDebug() << "Launching " << executable;
		QProcess::startDetached( executable );
		hide();
	}
	void executeCommand() {
		QString command = launchEditor->currentText();
		if ( !command.isEmpty() ) {
			QProcess::startDetached( command );
			if ( launchEditor->findText( command )==-1 )
				launchEditor->addItem( command );
		}
		hide();
	}
	void logout() {
		QCopChannel::send( "org.qde.server", "quit" );
		hide();
	}
	void restart() {
		QCopChannel::send( "org.qde.qdm.system", "restart" );
		hide();
	}
	void halt() {
		QCopChannel::send( "org.qde.qdm.system", "halt" );
		hide();
	}
private:
	QList< QList<QListWidgetItem*> > menuItemsList;
	QListWidget *categoryViewer;
	QListWidget *itemsViewer;
	QListWidget *favoritesViewer;
	QComboBox *launchEditor;
};

class Qurumi: public QObject {
	Q_OBJECT
public:
	Qurumi() {
		QCopChannel *channel = new QCopChannel( "org.qde.plugins.qurumi", this );
		connect(channel, SIGNAL(received(const QString&,const QByteArray&)),
			this, SLOT(eventMenu(const QString&,const QByteArray&)) );
	}
private slots:
	void eventMenu( const QString& message, const QByteArray& data ) {
		QDataStream stream(data);
		if ( message == "showRect" ) {
			QRect r;
			stream >> r;
			menu.setGeometry( adjustToDesktop( r ) );
			menu.show();
		} else if ( message == "showAt" ) {
			QPoint pos;
			stream >> pos;
			menu.move( pos );
			menu.setGeometry( adjustToDesktop( menu.geometry() ) );
			menu.show();
		} else if ( message == "show" ) {
			menu.show();
		} else if ( message == "hide" ) {
			menu.hide();
		}
	}
private:
	QurumiMenu menu;
};

DECLARE_QDESKTOP_CREATOR() {
	(void)parameters; // Supress 'unused parameter...' warning
	return new Qurumi();
}

#include "qurumi_plug.moc"
