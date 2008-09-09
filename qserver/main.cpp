#include <QApplication>
#include <QWSServer>

#include "qserver.h"

int main( int argc, char **argv ) {
    QApplication app( argc, argv, QApplication::GuiServer );
    QDEServer server;
    QWSServer::instance()->refresh();
    app.setQuitOnLastWindowClosed( false );
    return app.exec();
}
