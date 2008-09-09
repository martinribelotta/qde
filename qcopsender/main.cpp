#include <QApplication>
#include <QCopChannel>


int usage() {
    qDebug( "usage: qcopmessage [channel] [message] <data>\n" );
    return 1;
}

int main( int argc, char **argv ) {
    QApplication app( argc, argv );
    if ( argc < 2 )
	return usage();
    QByteArray data;
    for ( int i=3; i<argc; i++ ) {
        data.append( argv[i] );
        data.append( ' ' );
    }
    QCopChannel::send( argv[1], argv[2], data );
    return 0;
}
