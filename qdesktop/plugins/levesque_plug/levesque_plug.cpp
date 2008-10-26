#include <QWidget>
#include <QSettings>
#include <QApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include <QComboBox>
#include <QHideEvent>
#include <QPainter>
#include <QDataStream>
#include <QMessageBox>
#include <QCopChannel>

#include <QtDebug>

#include "../include/plugins.h"

//#define QDE_BASEDIR QString(QApplication::applicationDirPath()+"/../")

class LevesqueWidget: public QWidget {
	Q_OBJECT
public:
	//enum FadeDirection { FadeIn, FadeOut };

	LevesqueWidget() {
		setWindowFlags( Qt::Popup );
		setWindowOpacity( 0.8 );
		//fadeTimer = new QTimer( this );
		//connect( fadeTimer, SIGNAL(timeout()), this, SLOT(fadeTimer_step()) );
		makeGui();
		//resize( sizeHint() );
		loadCommandHistory();
	}

	~LevesqueWidget() {
		saveCommandHistory();
	}

	/*virtual void doHide() {
		direction = FadeOut;
		fadeTimer->start(10);
	}*/

protected:
	void paintEvent( QPaintEvent* ) {
		QPainter p( this );
		p.setPen( Qt::black );
		p.drawRect( QRect(0,0,width()-1,height()-1) );
	}

	void makeGui() {
		QRect dr = QApplication::desktop()->geometry();
		QVBoxLayout *layout = new QVBoxLayout( this );
		launchEditor = new QComboBox( this );
		launchEditor->setEditable( true );
		layout->addWidget( launchEditor );
		layout->addStretch( 1 );
		setFixedSize( 400, 200 );
		QRect r = geometry();
		r.moveCenter( dr.center() );
		move( r.topLeft() );
		launchEditor->setFocus();
		connect(launchEditor->lineEdit(), SIGNAL(returnPressed()),
			this, SLOT(executeCommand()) );
	}

	/*virtual void showEvent( QShowEvent* *//*e*//* ) {
		if ( windowOpacity() == 0.0 ) {
			direction = FadeIn;
			fadeTimer->start(10);
		}
	}*/

	/*virtual void mousePressEvent( QMouseEvent* event ) {
		event->accept();
		QWidget* w;
		while ((w = qApp->activePopupWidget()) && w != this) {
			w->close();
			if (qApp->activePopupWidget() == w) // widget does not want to dissappear
				w->hide(); // hide at least
		}
		if (!rect().contains(event->pos()))
			doHide(); //close();
	}*/

	/*virtual void hideEvent( QHideEvent* *//*e*//* ) {
		setWindowOpacity(0.0);
	}*/

	QString commandHistoryFilename() {
		return QSettings( "qde", "levesque_history" ).fileName();
	}

public slots:
	/*void fadeTimer_step() {
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
	}*/

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
	void executeCommand() {
		QString cmdLine = launchEditor->currentText();
		if ( !cmdLine.isEmpty() ) {
			QProcess::startDetached( cmdLine );
			if ( launchEditor->findText( cmdLine )==-1 )
				launchEditor->addItem( cmdLine );
		}
		hide();
	}

private:
	QComboBox *launchEditor;

	/*QTimer *fadeTimer;
	FadeDirection direction;*/
};

class Levesque: public QObject {
	Q_OBJECT
public:
	Levesque() {
		levesque.hide();
		connect(new QCopChannel( "org.qde.qserver.events", this ),
			SIGNAL(received(const QString&,const QByteArray&)),
			this, SLOT(filter(const QString&,const QByteArray&)) );
	}

public slots:
	virtual void filter( const QString& message, const QByteArray& data ) {
		qDebug() << message;
		if ( message == "keyevent" ) {
			int unicode;
			int keycode;
			int modifiers;
			int isPress_int;
			int autoRepeat_int;
			QDataStream stream( data );
			stream >> unicode >> keycode >> modifiers >> isPress_int >> autoRepeat_int;
			if ((static_cast<bool>(isPress_int)) &&
			    (keycode == Qt::Key_Space) &&
			    (modifiers == Qt::AltModifier) ) {
				//QMessageBox::information( 0l, "info", "key press" );
				levesque.show();
			}
		}
	}

private:
	LevesqueWidget levesque;
};

DECLARE_QDESKTOP_CREATOR() {
	(void)parameters; // Supress 'unused parameter...' warning
	return new Levesque();
}

#include "levesque_plug.moc"
