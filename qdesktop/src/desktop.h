#ifndef QDE_DESKTOP_H
#define QDE_DESKTOP_H

#include <QWidget>
#include <QPixmap>

class QGridLayout;

class QDEsktop: public QWidget {
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
private:
	QPixmap background;
	QGridLayout *layout;
};

#endif /* QDE_DESKTOP_H */
