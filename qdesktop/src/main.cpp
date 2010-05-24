#include <QApplication>

#include "desktop.h"
#include "bottombar.h"
#include "pluginloader.h"

//#include "qlauncher.h"

int main( int argc, char **argv ) {
	QApplication app( argc, argv );

	QDEPluginLoader pluginLoader;
	pluginLoader.loadAll();

	QDEsktop desktop;
	desktop.showFullScreen();

	/*QDELauncher launcher;
	launcher.show();*/

	BottomBar bar;
	bar.show();

	return app.exec();
}

//#include "main.moc"
