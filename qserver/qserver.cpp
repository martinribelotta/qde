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
#include "qserver_p.h"

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

bool QDEKeyboardFilter::filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat ) {
    QByteArray data;
    QDataStream stream( &data, QIODevice::WriteOnly );
    int isPress_int = isPress;
    int autoRepeat_int = autoRepeat;
    stream << unicode << keycode << modifiers << isPress_int << autoRepeat_int;
    QCopChannel::send( "org.qde.qserver.events", "keyevent", data );
        return false;
};

QDEServerPrivate::QDEServerPrivate( QObject *parent ):QObject(parent) {
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

QDEServerPrivate::~QDEServerPrivate() {
}

void QDEServerPrivate::message( const QString& message, const QByteArray& data ) {
    if ( message == "quit" )
        quitFromQDE();
    if ( message == "setMaxWindowRect" ) {
        QDataStream stream( data );
        QRect r;
        stream >> r;
        QWSServer::instance()->setMaxWindowRect( r );
    }
    if ( message.startsWith( "wm:" ) ) {
        quint32 wid;
        QDataStream stream( data );
        stream >> wid;
        QWSWindow *window = findWindow( wid );
        if ( window ) {
            if ( message == "wm:setActiveWindow" ) {
                window->show();
                window->setActiveWindow();
            // } else if ( message == "wm::raise" ) {
                window->raise();
            }
        }
    }
}

void QDEServerPrivate::windowEvent( QWSWindow *window, QWSServer::WindowEvent event ) {
    sendWindowEvent( window, event );
}

void QDEServerPrivate::quitFromQDE() {
    int ret = QMessageBox::question( 0l,
            tr("Question"), tr("Quit from QDE?"),
            QMessageBox::Yes, QMessageBox::No );
    if ( ret == QMessageBox::Yes )
        QApplication::instance()->quit();
}

void QDEServerPrivate::executeRcFile() {
    QString rcFileName = sets->fileName().replace( ".ini", ".rc" );
    if ( !QFile::exists( rcFileName ) )
        rcFileName = QApplication::applicationDirPath()+"/../Settings/qde/qdeserver.rc";
    QProcess::startDetached( QString("/bin/sh %1").arg( rcFileName ) );
}

static QString oldStyle;

QDEServer::QDEServer() {
    d = new QDEServerPrivate( QApplication::instance() );
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
