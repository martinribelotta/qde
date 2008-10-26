#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QMessageBox>
#include <QCopChannel>
#include <QWSServer>
#include <QDesktopWidget>
#include <QWSWindow>

#include <QtDebug>

#include "qserver.h"

#include "../qdesktop/plugins/include/qwswindow.h"

static inline QWSWindow *findWindow( int wid ) {
	foreach( QWSWindow* w, QWSServer::instance()->clientWindows() )
		if ( w->winId() == wid )
			return w;
	return 0l;
}

static inline void sendWindowEvent( QWSWindow *window, QWSServer::WindowEvent event ) {
	QByteArray data;
	QDataStream stream( &data, QIODevice::WriteOnly );
	stream << quint32(event) << QDEWindow(window).toData();
	QCopChannel::send( "org.qde.server.wm", "windowEvent", data );
}

class QDEKeyboardFilter: public QWSServer::KeyboardFilter {
public:
	virtual bool filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat ) {
		QByteArray data;
		QDataStream stream( &data, QIODevice::WriteOnly );
		int isPress_int = isPress;
		int autoRepeat_int = autoRepeat;
		stream << unicode << keycode << modifiers << isPress_int << autoRepeat_int;
		QCopChannel::send( "org.qde.qserver.events", "keyevent", data );
		return false;
	}
};

class QDEServerPrivate: public QObject {
    Q_OBJECT
public:
    QDEServerPrivate() {
	sets = new QSettings(QSettings::IniFormat,
	    QSettings::UserScope, "qde", "qdeserver", this );
	ipcChannel = new QCopChannel( "org.qde.server", this );
	connect(
	    ipcChannel, SIGNAL(received(const QString&,const QByteArray&)),
	    this, SLOT(message(const QString&,const QByteArray&))
	);
	connect(QWSServer::instance(), SIGNAL(windowEvent(QWSWindow*,QWSServer::WindowEvent)),
	    this, SLOT(windowEvent(QWSWindow*,QWSServer::WindowEvent)) );

	executeRcFile();
	QWSServer::addKeyboardFilter( new QDEKeyboardFilter() );
    }

    ~QDEServerPrivate() {
    }

private slots:
    void message( const QString& message, const QByteArray& data ) {
	QDataStream stream( data );
	if ( message == "quit" )
	    quitFromQDE();
	if ( message == "setMaxWindowRect" ) {
	    QRect r;
	    stream >> r;
	    QWSServer::instance()->setMaxWindowRect( r );
	}
	if ( message.startsWith( "wm:" ) ) {
	    quint32 wid;
	    stream >> wid;
	    QWSWindow *window = findWindow( wid );
	    if ( window ) {
		if ( message == "wm:setActiveWindow" ) {
		    window->show();
		    window->setActiveWindow();
		//else if ( message == "wm::raise" )
		    window->raise();
		}
	    }
	}
    }

    void windowEvent( QWSWindow *window, QWSServer::WindowEvent event ) {
	sendWindowEvent( window, event );
	/*QByteArray data;
	QDataStream stream( &data, QIODevice::WriteOnly );
	quint32 id = window->winId();
	quint32 ev = (quint32)event;
	quint32 flags = (quint32)window->windowFlags();
	//QRect r = window->paintedRegion().boundingRect();
	QString caption = window->caption();
	stream << id << ev << flags << caption; // << r;
	QCopChannel::send( "org.qde.server.wm", "windowEvent", data );*/
    }

public slots:
    void quitFromQDE() {
	int ret = QMessageBox::question( 0l,
	    tr("Question"), tr("Quit from QDE?"),
		QMessageBox::Yes, QMessageBox::No );
	if ( ret == QMessageBox::Yes )
	    QApplication::instance()->quit();
    }

private:
    void executeRcFile() {
	/*QFile rc( sets->fileName().replace( ".ini", ".rc" ) );
	if ( rc.open( QFile::ReadOnly ) )
	    while ( !rc.atEnd() )
		executeLine( rc.readLine() );*/
	QString rcFileName = sets->fileName().replace( ".ini", ".rc" );
	if ( !QFile::exists( rcFileName ) )
		rcFileName = QApplication::applicationDirPath()+"/../Settings/qde/qdeserver.rc";
	QProcess::startDetached( QString("/bin/sh %1").arg( rcFileName ) );
    }

    /*void executeLine( const QString& s ) {
	QProcess::startDetached( s );
    }*/

    QSettings *sets;
    QCopChannel *ipcChannel;
    QProcess *rcProc;
};

static QString oldStyle;

QDEServer::QDEServer() {
    d = new QDEServerPrivate();
    QSettings sets( "Trolltech" );
    sets.beginGroup( "Qt" );
    oldStyle = sets.value( "style", QString() ).toString();
    sets.setValue( "style", "qdestyle" );
    QApplication::setStyle( "qdestyle" );
}

QDEServer::~QDEServer() {
    QSettings sets( "", "Trolltech" );
    sets.beginGroup( "Qt" );
    sets.setValue( "style", oldStyle );
    delete d;
}

#include "qserver.moc"
