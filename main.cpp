#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle("Plastique");
	QTranslator translator;
	translator.load("ru_RU.qm");
	a.installTranslator(&translator);

	MainWindow w;
	w.show();

	return a.exec();
}
