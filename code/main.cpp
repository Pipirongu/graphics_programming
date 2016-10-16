#include "settingswin.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SettingsWin settings;
	settings.show();

	return a.exec();

}
