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
#include <QLineEdit>

#include <QStandardItem>
#include <QStandardItemModel>

#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <QTimer>

#include <QHideEvent>

#include <QCache>

#include <QtDebug>

#include "../include/plugins.h"

#define QDE_BASEDIR QString(QApplication::applicationDirPath()+"/../")
#define DEFAULT_META_ICON QDE_BASEDIR+"/share/icons/mimetypes/exec.png"
#define DEFAULT_META_TEXT "..."
#define DEFAULT_META_EXEC "/bin/true"
#define DEFAULT_META_TOOLTIP "Unknow"
#define DEFAULT_CATEGORY_ICON QDE_BASEDIR+"/share/icons/places/folder.png"

const QSize stdIconSize( 32, 32 );

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

struct CategoryItem {
	QIcon icon;
	QString text;
	QString exec;

	CategoryItem( const QIcon& _icon, const QString& _text, const QString& _exec ):
		icon(_icon), text(_text), exec(_exec) { }

	QStandardItem *makeItem() {
		QStandardItem *item = new QStandardItem( icon, text );
		item->setData( QVariant(exec), Qt::UserRole );
		return item;
	}
};

struct Category {
	QList<CategoryItem> itemList;
	QStandardItem *categoryItem;

	Category() {
		categoryItem = 0l;
	}

	bool isValid() { return categoryItem!=0l; }
};

class QurumiMenu: public QWidget {
	Q_OBJECT
public:
	enum FadeDirection { FadeIn, FadeOut };

	QurumiMenu() {
		setWindowFlags( Qt::Popup /*Qt::FramelessWindowHint*/ );
		setWindowOpacity( 0.0 );
		fadeTimer = new QTimer( this );
		connect( fadeTimer, SIGNAL(timeout()), this, SLOT(fadeTimer_step()) );
		makeGui();
		resize( sizeHint() );
		loadConfiguration();
		loadCommandHistory();
	}
	~QurumiMenu() {
		saveCommandHistory();
	}

	virtual void doHide() {
		direction = FadeOut;
		fadeTimer->start(10);
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

		connect(launchEditor->lineEdit(), SIGNAL(returnPressed()),
			this, SLOT(executeCommand()) );

		launchButton = new QToolButton(this);
		hLayout0->addWidget(launchButton);

		vLayout1->addLayout(hLayout0);

		QWidget *appWidget = new QWidget( this );
		categoryViewer = new QListView( appWidget );
		categoryModel = new QStandardItemModel( this );
		categoryViewer->setModel( categoryModel );
		categoryViewer->setIconSize( stdIconSize );
		categoryViewer->setUniformItemSizes( true );

		connect(categoryViewer->selectionModel(), SIGNAL(currentChanged(const QModelIndex&,const QModelIndex&)),
			this, SLOT(categoryChange(const QModelIndex&,const QModelIndex&)) );

		itemsViewer = new QListView( appWidget );
		itemsModel = new QStandardItemModel( this );
		itemsViewer->setModel( itemsModel );
		itemsViewer->setIconSize( stdIconSize );
		itemsViewer->setUniformItemSizes( true );

		connect(itemsViewer, SIGNAL(clicked(const QModelIndex&)),
			this, SLOT(executeItem(const QModelIndex&)) );

		hLayout2 = new QHBoxLayout( appWidget );
		hLayout2->setContentsMargins( 0,0,0,0 );
		hLayout2->addWidget(categoryViewer);
		hLayout2->addWidget(itemsViewer);

		favoritesViewer = new QListView( this );
		favoritesModel = new QStandardItemModel( this );
		favoritesViewer->setModel( favoritesModel );
		favoritesViewer->setIconSize( stdIconSize );
		favoritesViewer->setUniformItemSizes( true );

		connect(favoritesViewer, SIGNAL(clicked(const QModelIndex&)),
			this, SLOT(executeItem(const QModelIndex&)) );

		tab = new QTabWidget( this );
		tab->setIconSize( QSize(32,32) );
		tab->addTab( favoritesViewer, QIcon( ":/favorites.png" ), tr( "Favorites" ) );
		tab->addTab( appWidget, QIcon( ":/apps.png" ), tr( "Applications" ) );

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
		buttonRestart->setIcon(QIcon(QDE_BASEDIR+"/share/icons/actions/system-restart.png"));
		buttonRestart->setIconSize( QSize(32,32) );

		buttonShutdown->setText(tr( "Shutdown" ));
		buttonShutdown->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		buttonShutdown->setIcon(QIcon(QDE_BASEDIR+"/share/icons/actions/system-shutdown.png"));
		buttonShutdown->setIconSize( QSize(32,32) );

		buttonLogout->setText(tr( "Logout" ));
		buttonLogout->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		buttonLogout->setIcon(QIcon(QDE_BASEDIR+"/share/icons/actions/system-log-out.png"));
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

	QIcon cached( const QString& filename ) {
		if ( !iconCache.contains( filename ) )
			iconCache.insert( filename, new QIcon(
				QPixmap( filename ).scaled(
					stdIconSize,
					Qt::KeepAspectRatio,
					Qt::SmoothTransformation
				) ) );
		return *iconCache[filename];
		//return QIcon( filename );
	}

	static const QString commandHistoryFilename() {
		QSettings sets( "qde", "qurumi_history" );
		return sets.fileName();
	}

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
			CategoryItem item = createItem( qdesk );
			if ( categoryName == "favorites" ) {
				favoritesModel->appendRow( item.makeItem() );
			} else {
				Category& category = findOrNewCategory( categoryName );
				category.itemList.append( item );
			}
		}
	}

	CategoryItem createItem( QSettings& qdesk ) {
		QString text = qdesk.value( "text", DEFAULT_META_TEXT ).toString();
		QIcon icon( qdesk.value( "icon", DEFAULT_META_ICON ).toString() );
		QString exec = qdesk.value( "exec", DEFAULT_META_EXEC ).toString();
		CategoryItem item( icon, text, exec );
		return item;
	}

	Category& findOrNewCategory( const QString& categoryName ) {
		Category &categoryEntry = categoryList[ categoryName ];
		if ( !categoryEntry.isValid() ) {
			categoryEntry.categoryItem = new QStandardItem(
				cached(DEFAULT_CATEGORY_ICON),
				categoryName);
			categoryModel->appendRow( categoryEntry.categoryItem );
		}
		return categoryEntry;
	}

	virtual void showEvent( QShowEvent* /*e*/ ) {
		if ( windowOpacity() == 0.0 ) {
			direction = FadeIn;
			fadeTimer->start(10);
		}
	}

	virtual void mousePressEvent( QMouseEvent* event ) {
		event->accept();
		QWidget* w;
		while ((w = qApp->activePopupWidget()) && w != this) {
			w->close();
			if (qApp->activePopupWidget() == w) // widget does not want to dissappear
				w->hide(); // hide at least
		}
		if (!rect().contains(event->pos()))
			doHide(); //close();
	}

	virtual void hideEvent( QHideEvent* /*e*/ ) {
		setWindowOpacity(0.0);
	}

public slots:
	void fadeTimer_step() {
		double opacity = windowOpacity();
		const double fadeStep = 0.18;
		switch ( direction ) {
			case FadeIn:
				opacity += fadeStep;
				if ( opacity > 1.0 ) {
					opacity = 1.0;
					fadeTimer->stop();
				}
			break;
			case FadeOut:
				opacity -= fadeStep;
				if ( opacity < 0.0 ) {
					opacity = 0.0;
					fadeTimer->stop();
					hide();
				}
			break;
		}
		setWindowOpacity( opacity );
	}

	void loadCommandHistory() {
		QFile file( commandHistoryFilename() );
		if ( file.open( QFile::ReadOnly ) ) {
			while ( !file.atEnd() )
				launchEditor->addItem(
					QString::fromLocal8Bit(file.readLine().replace( '\n', '\0' )) );
		}
	}

	void saveCommandHistory() {
		QFile file( commandHistoryFilename() );
		if ( file.open( QFile::WriteOnly ) )
			for( int i=0; i<launchEditor->count(); i++ ) {
				QString cmd = launchEditor->itemText(i);
				file.write( cmd.toLocal8Bit() );
				file.write( "\n" );
			}
	}

private slots:
	void categoryChange( const QModelIndex& index, const QModelIndex& /*old*/ ) {
		QStandardItem *curr = categoryModel->itemFromIndex( index );
		if ( curr )  {
			QString categoryName = curr->text();
			QList<CategoryItem> &itemList = categoryList[ categoryName ].itemList;
			itemsModel->clear();
			foreach( CategoryItem categoryItem, itemList )
				itemsModel->appendRow( categoryItem.makeItem() );
		} else
			qDebug() << "BUG: categoryChange null index";
	}

	void executeItem( const QModelIndex& index ) {
		QStandardItem *item = itemsModel->itemFromIndex( index );
		if ( !item )
			item = favoritesModel->itemFromIndex( index );
		if ( item ) {
			QString executable = item->data( Qt::UserRole ).toString();
			qDebug() << "Launching " << executable;
			QProcess::startDetached( executable );
		} else
			qDebug() << "BUG: executeItem null index";
		doHide();
	}

	void executeCommand() {
		QString cmdLine = launchEditor->currentText();
		if ( !cmdLine.isEmpty() ) {
			QProcess::startDetached( cmdLine );
			if ( launchEditor->findText( cmdLine )==-1 )
				launchEditor->addItem( cmdLine );
		}
		doHide();
	}

	void logout() {
		QCopChannel::send( "org.qde.server", "quit" );
		doHide();
	}

	void restart() {
		QCopChannel::send( "org.qde.qdm.system", "restart" );
		doHide();
	}

	void halt() {
		QCopChannel::send( "org.qde.qdm.system", "halt" );
		doHide();
	}
private:
	QCache<const QString,QIcon> iconCache;
	QHash<const QString,Category> categoryList;

	QListView *categoryViewer;
	QStandardItemModel *categoryModel;

	QListView *itemsViewer;
	QStandardItemModel *itemsModel;

	QListView *favoritesViewer;
	QStandardItemModel *favoritesModel;

	QComboBox *launchEditor;

	QTimer *fadeTimer;
	FadeDirection direction;
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
			menu.doHide();
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
