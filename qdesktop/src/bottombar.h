#ifndef QDE_BOTTOMBAR_H
#define QDE_BOTTOMBAR_H

#include <QWidget>
#include <QList>

class QHBoxLayout;

class BottomBar: public QWidget {
	Q_OBJECT
public:
	BottomBar();
	~BottomBar();

protected:
	virtual void paintEvent( QPaintEvent *e );

public slots:
	void clearPlugins();
	void loadPlugins();
	void loadConfig();

private slots:
	void reconfigure( const QString&, const QByteArray& );

private:
	QList<QWidget*> pluginWidgets;
	QHBoxLayout *layout;
};

#endif /* QDE_BOTTOMBAR_H */
