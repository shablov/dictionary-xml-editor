#include "iconproxystyle.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle("Plastique");
	QIcon::setThemeName("HighContrast");
	QString translationsPath(QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	QLocale locale = QLocale::system();

//	material
//	gnome
//	HighContrast

	QTranslator qtTranslator;
	if (qtTranslator.load(locale, "qt", "_", translationsPath))
	{
		a.installTranslator(&qtTranslator);
	}

	QTranslator qtBaseTranslator;
	if (qtBaseTranslator.load(locale, "qtbase", "_", translationsPath))
	{
		a.installTranslator(&qtBaseTranslator);
	}

	QTranslator dictionaryTranslator;
	if (dictionaryTranslator.load(locale, ":/files/dictionary_translator", "_"))
	{
		a.installTranslator(&dictionaryTranslator);
	}

	MainWindow w;
	w.show();

	return a.exec();
}
