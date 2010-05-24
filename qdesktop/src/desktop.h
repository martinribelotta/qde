#ifndef QDE_DESKTOP_H
#define QDE_DESKTOP_H

#include <QWidget>
#include <QPixmap>

class QDEDesktopItem;

class QDEsktop: public QWidget
{
	Q_OBJECT
public:
	QDEsktop();

protected:
	virtual void paintEvent( QPaintEvent *e );
	virtual void showEvent( QShowEvent *e );
	virtual void mousePressEvent( QMouseEvent *e );

public slots:
	void loadBackground();
	void loadMoqoids();
	void loadFileIcons();
	//void adjustItems();
	void adjustItem( QDEDesktopItem* item, const QPoint& p );

private slots:
	void reconfigure( const QString&, const QByteArray& );

private:
	QPixmap background;
	int gridX, gridY;
	QList<QDEDesktopItem*> itemList;
};

#endif /* QDE_DESKTOP_H */
