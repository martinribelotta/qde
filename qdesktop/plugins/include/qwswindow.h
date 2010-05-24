#ifndef QDE_QWS_WINDOW_H
#define QDE_QWS_WINDOW_H

#include <QWSWindow>
#include <QWSServer>
#include <QByteArray>
#include <QDataStream>

struct QDEWindow {
    int winId;
    QString name;
    QString caption;
    struct {
        int id;
        QString name;
    } client;
    QRegion requestedRegion;
    bool isVisible;
    bool isPartialyObscured;
    bool isFullyObscured;
    QWSWindow::State state;
    Qt::WindowFlags windowFlags;

    QDEWindow( QWSWindow *w ) {
        winId = w->winId();
        name = w->name();
        caption = w->caption();
        if ( w->client() ) {
            client.id = w->client()->clientId();
            client.name = w->client()->identity();
        } else {
            client.id = -1;
            client.name = "unknow";
        }
        requestedRegion = w->requestedRegion();
        isVisible = w->isVisible();
        isPartialyObscured = w->isPartiallyObscured();
        isFullyObscured = w->isFullyObscured();
        state = w->state();
        windowFlags = w->windowFlags();
    }

    QDEWindow( const QByteArray& data ) {
        QDataStream stream( data );
        quint32 quint32_state, quint32_windowFlags;
        stream  >> winId
                >> name
                >> caption
                >> client.id
                >> client.name
                >> requestedRegion
                >> isVisible
                >> isPartialyObscured
                >> isFullyObscured
                >> quint32_state
                >> quint32_windowFlags;
        state = static_cast<QWSWindow::State>(quint32_state);
        windowFlags = static_cast<Qt::WindowFlags>(quint32_windowFlags);
    }

    QByteArray toData() {
        QByteArray data;
        QDataStream stream( &data, QIODevice::WriteOnly );
        stream  << winId
                << name
                << caption
                << client.id
                << client.name
                << requestedRegion
                << isVisible
                << isPartialyObscured
                << isFullyObscured
                << quint32(state)
                << quint32(windowFlags);
        return data;
    }

private:
    QDEWindow() { }
};

#endif /* QDE_QWS_WINDOW_H */
