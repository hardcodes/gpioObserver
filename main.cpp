// gpioObserver, (c)2013 by Sven Putze
// GPL
#include <QCoreApplication>
#include <QTimer>
#include <signal.h>
#include <stdio.h>
#include "mainclass.h"

using namespace std;


int main(int argc, char *argv[])
{
	signal(SIGINT, QCoreApplication::exit);

	QCoreApplication app(argc, argv);
	MainClass mainClass;
	QObject::connect(&mainClass, SIGNAL(finished()), &app, SLOT(quit()));
	QObject::connect(&app, SIGNAL(aboutToQuit()), &mainClass, SLOT(aboutToQuitApp()));
	QTimer::singleShot(10, &mainClass, SLOT(run()));
	return app.exec();
}


