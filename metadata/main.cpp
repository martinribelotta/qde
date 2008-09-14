#include <QApplication>

#include "qonfiguratorwidgets/configdialog.h"

int main( int argc, char **argv ) {
	QApplication app(argc,argv);
	ConfigDialog w;
	w.show();
	return app.exec();
}
