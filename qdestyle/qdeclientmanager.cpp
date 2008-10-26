#include "qdeclientmanager.h"

#include <QCopChannel>
#include <QByteArray>
#include <QDataStream>
#include <QThread>
#include <QApplication>
#include <QWidget>

#include <QtDebug>

static quint32 currentThreadId=0l;

static inline QWidget *findWindowById( quint32 id ) {
	foreach( QWidget *w, QApplication::allWidgets() )
		if ( w->winId() == id )
			return w;
	return 0l;
}

QDEClientManager::QDEClientManager( QObject *parent ): QObject( parent )
{
	QByteArray data;
	QDataStream stream( &data, QIODevice::WriteOnly );
	if ( !currentThreadId )
		currentThreadId = (quint32)QThread::currentThreadId();
	stream << currentThreadId;
	QCopChannel::send( "org.qde.clients", "clientConnected", data );
	connect(new QCopChannel( "org.qde.clients", this ),
		SIGNAL(received(const QString&,const QByteArray&)),
		this,
		SLOT(received(const QString&,const QByteArray&)) );
}

QDEClientManager::~QDEClientManager()
{
	QByteArray data;
	QDataStream stream( &data, QIODevice::WriteOnly );
	stream << currentThreadId;
	QCopChannel::send( "org.qde.clients", "clientDisconnect", data );
}

void QDEClientManager::received( const QString& message, const QByteArray& data )
{
	QDataStream stream( data );
	quint32 id;
	stream >> id;
	qDebug() << "message client received" << message << "from" << id;
//	qDebug() << "currentThreadId"<< currentThreadId;
//	if ( id == currentThreadId ) {
		if ( message == "moveWindow" ) {
			quint32 wid, x, y;
			stream >> wid >> x >> y;
			QWidget *w = findWindowById( wid );
			if ( w )
				w->move( x, y );
			else
				qDebug() << "QDEClientManager error: Window id not found:" << wid;
		}
//	}
}
