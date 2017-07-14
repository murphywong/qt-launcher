#include "mainwindow.h"

#include <QApplication>
#include <QtDebug>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QTextCodec>
#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
	qDebug() << "codec for locale: " << QTextCodec::codecForLocale()->name();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForCStrings(codec);
#endif

	ui::MainWindow *window = new ui::MainWindow;
	window->start();

	app.exec();

	delete window;
}
