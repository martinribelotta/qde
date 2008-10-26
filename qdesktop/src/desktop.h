#ifndef QDE_DESKTOP_H
#define QDE_DESKTOP_H

//#define QDESKTOP_EXPERIMENTAL

#include <QWidget>

#ifdef QDESKTOP_EXPERIMENTAL
#include <QGraphicsView>
#else
#include <QPixmap>
#endif

class QDEsktop: public
#ifdef QDESKTOP_EXPERIMENTAL
	QGraphicsView
#else
	QWidget
#endif
{
	Q_OBJECT
public:
	QDEsktop();

protected:
	virtual void paintEvent( QPaintEvent *e );
	virtual void showEvent( QShowEvent *e );

public slots:
	void loadBackground();
	void loadMoqoids();

private slots:
	void reconfigure( const QString&, const QByteArray& );

#ifndef QDESKTOP_EXPERIMENTAL
private:
	QPixmap background;
#endif
};

#endif /* QDE_DESKTOP_H */
