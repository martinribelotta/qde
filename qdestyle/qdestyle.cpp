#include <QSettings>
#include <QCopChannel>
#include <QStylePlugin>
#include <QStyleFactory>
#include <QApplication>
#include <QWidget>

#include "qdestyle.h"
#include "qdeclientmanager.h"

#include <QtDebug>

#define QDESTYLE_NAME "qdestyle"

static QDEClientManager *clientManager = 0l;

QDEStyle::QDEStyle():
	QDEProxyStyle( getLoadStyle() )
{
	connect(new QCopChannel( "org.qde.qonfigurator.style", this ),
		SIGNAL(received(const QString&,const QByteArray&)),
		this,
		SLOT(reconfigureStyles(const QString&,const QByteArray&)) );
	if ( !clientManager )
		clientManager = new QDEClientManager( this );
}

QDEStyle::~QDEStyle()
{
	if ( clientManager ) {
		delete clientManager;
		clientManager = 0l;
	}
}

void QDEStyle::loadStyle() {
	/*qDebug() << "1";
	QDEStyle *instance = static_cast<QDEStyle*>(QApplication::style());
	QApplication::setStyle( instance->managedStyle() );
	qDebug() << "2";
	//setManagedStyle( getLoadStyle() );
	qDebug() << "3";
	QApplication::setStyle( new QDEStyle() );
	//QApplication::setStyle( QApplication::style() );
	qDebug() << "4";
	QApplication::setPalette( QApplication::style()->standardPalette() );
	qDebug() << "5";*/
	QApplication::setStyle( QDESTYLE_NAME );
	QApplication::setPalette( QApplication::style()->standardPalette() );
	foreach( QWidget *w, QApplication::topLevelWidgets() )
		w->update();
}

QStyle *QDEStyle::getLoadStyle() {
	QSettings sets( "qde", "desktop" );
	if ( !sets.contains( "style" ) )
		sets.setValue( "style", "plastique" );
	QString styleName = sets.value( "style" ).toString();
	QStyle *style = QStyleFactory::create( styleName );
	if ( !style ) {
		qDebug() << "FATAL!!!!!: style" << styleName << "can't created";
		style = QStyleFactory::create( "plastique" );
	}
	return style;
}

void QDEStyle::setManagedStyle( QStyle *managed )
{
	deleteManagedStyle();
	QDEProxyStyle::setManagedStyle( managed );
}

void QDEStyle::reconfigureStyles( const QString& message, const QByteArray& /*data*/ )
{
	if ( message == "reconfigure" )
		QDEStyle::loadStyle();
}

class QDEStylePlugin : public QStylePlugin
{
public:
    QDEStylePlugin() { ; }
    QStringList keys() const {
        return QStringList() << QDESTYLE_NAME;
    }
    QStyle *create(const QString &key) {
        if (key.toLower() == QDESTYLE_NAME) {
            return new QDEStyle();
        }
        return 0;
    }
};

Q_EXPORT_PLUGIN(QDEStylePlugin)
