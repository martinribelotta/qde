#include <QLCDNumber>
#include <QTimer>
#include <QTime>
#include <QCalendarWidget>
#include <QApplication>
#include <QDesktopWidget>

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

class ClockWidget: public QLCDNumber {
	Q_OBJECT
public:
	ClockWidget() {
		setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );
		
		calendar.resize( calendar.sizeHint() );
		calendar.setWindowFlags( Qt::FramelessWindowHint|Qt::Popup );
		calendar.hide();
		
		setSegmentStyle(Filled);
		QTimer *timer = new QTimer( this );
		connect( timer, SIGNAL(timeout()), this, SLOT(updateTime()) );
		timer->start(1000);
		updateTime();
	}
protected:
	virtual void mousePressEvent( QMouseEvent* /*e*/ ) {
		calendar.setGeometry( adjustToDesktop(
			QRect( ((QWidget*)parent())->mapToGlobal( pos() ),
				calendar.geometry().size() )
			) );
		calendar.show();
	}
public slots:
	void showCalendar() {
		
	}
private slots:
	void updateTime() {
		QTime time = QTime::currentTime();
		QString text = time.toString("hh:mm");
		if ( (time.second()%2) == 0 )
			text[2] = ' ';
		display(text);
	}
private:
	QCalendarWidget calendar;
};

DECLARE_QUIQUE_CREATOR() {
	return new ClockWidget();
}

#include <clock_plug.moc>
