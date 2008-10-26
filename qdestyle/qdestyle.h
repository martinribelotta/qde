#ifndef QDESTYLE_H
#define QDESTYLE_H

#include <QtCore/QString>
#include <QtCore/QByteArray>

#include "qdeproxystyle.h"

class QDEStyle: public QDEProxyStyle {
	Q_OBJECT
public:
	QDEStyle();
	virtual ~QDEStyle();

//public slots:
	static void loadStyle();

protected:
	virtual void setManagedStyle( QStyle *managed );

private slots:
	void reconfigureStyles( const QString& message, const QByteArray& data );

private:
	QStyle *getLoadStyle();
};

#endif /* QDESTYLE_H */
