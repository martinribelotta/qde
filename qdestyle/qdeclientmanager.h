#ifndef QDE_CLIENT_MANAGER_H
#define QDE_CLIENT_MANAGER_H

#include <QObject>

class QDEClientManager: public QObject {
	Q_OBJECT
public:
	QDEClientManager( QObject *parent=0l );
	~QDEClientManager();
private slots:
	void received( const QString&, const QByteArray& );
};

#endif /* QDE_CLIENT_MANAGER_H */
