#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include <QCoreApplication>
#include <QTextStream>
#include <QString>
#include <QTimer>
#include <QStringList>
#include <QProcess>

#include "GPIOClass.h"

const int timerInterval = 200;

class MainClass : public QObject
{
	Q_OBJECT
private:
	QCoreApplication *app;
	QString gpioPinNumber;
	QString externalCommand;
	QStringList argumentList;
	GPIOClass *gpioClass;
	std::string lastInputState;
	std::string stateToObserve;
	QTimer *observerTimer;
	QProcess *process;
public:
	explicit MainClass(QObject *parent = 0);
	~MainClass();
	void finish();
	void finishWithError(int returnCode = 0);
private:
	void usage();
	void parseCommandLine();
	void initGpio();
	void executeCommandLine();
signals:
	void finished();
	void finishedWithError(int returnCode =0);
public slots:
	void run();
	void aboutToQuitApp();
private slots:
	void fireTimer();
};

#endif // MAINCLASS_H
