/* Copyright (C) 1995-1997-2008 by GAINAX */
#include <QWidget>
#include <QToolButton>
#include <QPainter>

#include "../include/plugins.h"

class MyWidget: public QWidget {
public:
	MyWidget() {
		setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );
	}
protected:
	virtual void paintEvent( QPaintEvent * e ) {
		QPainter p( this );
		p.fillRect( QRect(0,0,width(),height()), QBrush( Qt::red ) );
	}
};

DECLARE_QUIQUE_CREATOR() {
	return new MyWidget();
}
