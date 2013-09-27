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
#include <memory>

// 5 times a second, fast enough to catch opening/closing a door/window
const int HC_TIMER_INTERVAL = 200;
// can only be on/off
const QRegExp HC_VALID_IO_STATES = QRegExp("^(1|0)$");
// there are more gpio pins but these are not preset with other functions
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
    // not using std::unique_ptr<> here because signals slots can not be connected (at least with Qt4.x
    // and the old QObject::connect syntax
    QTimer *observerTimer;
    QProcess *process;
public:
    explicit MainClass(QObject *parent = nullptr);
	~MainClass();
	void finish();
	void finishWithError(int returnCode = 0);
private:
	void usage();
    bool parseCommandLine();
    bool parseAndValidateCommandLine();
	void initGpio();
    void executeExternalProcess();
    std::string now();
signals:
	void finished();
	void finishedWithError(int returnCode =0);
public slots:
	void run();
	void aboutToQuitApp();
private slots:
	void fireTimer();
    void printCommandLineStandardOutput();
    void printCommandLineErrorOutput();
    void externalProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // MAINCLASS_H
