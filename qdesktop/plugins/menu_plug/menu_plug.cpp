#include <QWidget>
#include <QToolButton>
#include <QPainter>
#include <QApplication>
#include <QEvent>

#include <QCopChannel>
#include <QDataStream>
#include <QByteArray>

#include <QtDebug>

#include "../include/plugins.h"

class MenuButton: public QToolButton {
	Q_OBJECT
public:
	MenuButton() {
		setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
		setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		setIcon( QPixmap( ":/qde-logo-simple.svg" ) );
		setText( tr("Menu") );
		connect( this, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)) );
	}
private slots:
	void onClick(bool) {
		QPoint p = mapToGlobal( pos() );
		QByteArray data;
		QDataStream stream( &data, QIODevice::WriteOnly );
		stream << p;
		QCopChannel::send( "org.qde.plugins.qurumi", "showAt", data );
	}
};

DECLARE_QUIQUE_CREATOR() {
	return new MenuButton();
}

#include "menu_plug.moc"
