#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle("Plastique");
	a.setApplicationName(a.tr("Dictionary editor"));
	MainWindow w;
	w.show();

	return a.exec();
}
