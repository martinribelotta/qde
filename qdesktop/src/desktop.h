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
private:
	void loadBackground();
	void loadMoqoids();
	QPixmap background;
	QGridLayout *layout;
};

#endif /* QDE_DESKTOP_H */
