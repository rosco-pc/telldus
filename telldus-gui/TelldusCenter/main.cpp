#include "tellduscenterapplication.h"
#include "mainwindow.h"

#ifdef Q_WS_MAC
#include "CocoaInitializer.h"
#include "SparkleAutoUpdater.h"
#endif

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE( resource );

	TelldusCenterApplication application(argc, argv);

	MainWindow *w = new MainWindow();
	w->show();

#ifdef Q_WS_MAC
	AutoUpdater* updater = 0;
	CocoaInitializer initializer;
	updater = new SparkleAutoUpdater("file:///Users/micke/Documents/dev/appcast.xml");
	if (updater) {
		//updater->checkForUpdates();
	}
#endif

	return application.exec();
}