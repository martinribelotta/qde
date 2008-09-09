#include <QToolButton>
#include <QCopChannel>

#include "../include/plugins.h"

class QuitButton: public QToolButton {
	Q_OBJECT
public:
	QuitButton() {
		setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
		setIcon( QPixmap( ":/quitb_plug.svg" ) );
		setText( tr("Quit") );
		connect( this, SIGNAL(clicked()), this, SLOT(onQuit()) );
	}
public slots:
	void onQuit() {
		QCopChannel::send( "org.qde.server", "quit" );
	}
};

DECLARE_QUIQUE_CREATOR() {
	return new QuitButton();
}

#include "quitb_plug.moc"
