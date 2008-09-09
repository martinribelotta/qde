#ifndef QDE_BOTTOMBAR_H
#define QDE_BOTTOMBAR_H

#include <QWidget>

class BottomBar: public QWidget {
public:
	BottomBar();
	~BottomBar();
protected:
	virtual void paintEvent( QPaintEvent *e );
};

#endif /* QDE_BOTTOMBAR_H */
