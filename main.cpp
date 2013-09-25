// gpioObserver, (c)2013 by Sven Putze (hardcodes.de), see
//   http://
// GPIOClass, (c) Hussam Al-Hertani, see
//   https://github.com/halherta/RaspberryPi-GPIOClass-v1 and/or
//   http://hertaville.com/2012/11/18/introduction-to-accessing-the-raspberry-pis-gpio-in-c/
//
// This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.


#include <QCoreApplication>
#include <QTimer>
#include "mainclass.h"

using namespace std;

int main(int argc, char *argv[])
{
	signal(SIGINT, QCoreApplication::exit);

	QCoreApplication app(argc, argv);
	MainClass mainClass;
    // there is only Qt4.x on Raspberry Pi, using the old sytnax
	QObject::connect(&mainClass, SIGNAL(finished()), &app, SLOT(quit()));
	QObject::connect(&app, SIGNAL(aboutToQuit()), &mainClass, SLOT(aboutToQuitApp()));
	QTimer::singleShot(10, &mainClass, SLOT(run()));
	return app.exec();
}


