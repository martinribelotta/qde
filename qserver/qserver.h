#ifndef QDE_QSERVER_H
#define QDE_QSERVER_H

class QDEServerPrivate;

class QDEServer {
public:
    QDEServer();
    ~QDEServer();
private:
    QDEServerPrivate *d;
};

#endif /* QDE_QSERVER_H */

