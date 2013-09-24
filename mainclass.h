#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include <QCoreApplication>
#include <QTextStream>
#include <QString>
#include <QTimer>
#include <QStringList>
#include <QProcess>
#include <QRegExp>

#include "GPIOClass.h"

const int HC_TIMER_INTERVAL = 200;
const QRegExp HC_VALID_IO_STATES = QRegExp("^(1|0)$");
const QRegExp HC_VALID_GPIO_PINS = QRegExp("^(17|21|22|23|24|25|27)$");

class MainClass : public QObject
{
	Q_OBJECT
private:
	QCoreApplication *app;
	QString gpioPinNumber;
	QString externalCommand;
	QStringList argumentList;
    std::unique_ptr<GPIOClass> gpioClass;
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
    bool parseAndValidateCommandLine();
	void initGpio();
	void executeCommandLine();
    std::string now();
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
