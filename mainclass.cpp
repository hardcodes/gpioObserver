#include "mainclass.h"
#include <stdio.h>
#include <QDate>
#include <QFile>
#include <QTime>
#include <QByteArray>

using namespace std;

MainClass::MainClass(QObject *parent) :
	QObject(parent)
{
    app = QCoreApplication::instance();
    this->process = new QProcess(this);
    QObject::connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(printCommandLineStandardOutput()));
    QObject::connect(this->process, SIGNAL(readyReadStandardError()), this, SLOT(printCommandLineErrorOutput()));
    this->observerTimer = new QTimer(this);
	connect(this->observerTimer, SIGNAL(timeout()), this, SLOT(fireTimer()));
}

MainClass::~MainClass()
{
	delete this->observerTimer;
	delete this->process;
}

void MainClass::run()
{
    if(!parseCommandLine()){
        usage();
        finishWithError(1);
        return;
    }
    cout << now() << "waiting for next change to state " << this->stateToObserve << endl;
	initGpio();
	this->observerTimer->start(HC_TIMER_INTERVAL);
	// do not return, the eventloop keeps on running
}

void MainClass::finish()
{
	// you can do some cleanup here
	// then do emit finished to signal CoreApplication to quit
	emit finished();
}

void MainClass::finishWithError(int returnCode)
{
	app->exit(returnCode);
}

void MainClass::usage()
{
    cout << "gpioObserver v" << HC_PROGRAMVERSION << ", (c)2013 by Sven Putze [git hash: " << HC_GITHASH << "]" << endl << endl;
    cout << "  wait for one gpio pin to reach a desired state and execute a program afterwards" << endl << endl;
	cout << "usage:" << std::endl << endl;
    cout << "  gpioObserver <GPIO pin number> <state to observe> <executable> [<arguments>]" << endl << endl;
    cout << "    <GPIO pin number> must be one out of (17|21|22|23|24|25|27)" << endl;
    cout << "         21 is board revision 1" << endl;
    cout << "         27 is board revision 2 of the same pin" << endl;
    cout << "    <state to observe> must be one out of (0|1)" << endl;
    cout << "    <executable> is the file that should be executed if <state to observe> is reached" << endl;
    cout << "    [<arguments>] are optional commandline parameters for <executable>" << endl;
    cout << "         don't forget quotes if there is more then one parameter" << endl << endl;
}

bool MainClass::parseCommandLine()
{
    if((app->arguments().count()) < 4){
        cout << "too few arguments" << endl;
        return false;
    }
    return parseAndValidateCommandLine();
}

bool MainClass::parseAndValidateCommandLine()
{
    // Yes, there are more sophisticated ways to parse and check commandlines
    // A bunch of libraries addresses this problm
    // Just wanted to keep things simple here
    if(!HC_VALID_GPIO_PINS.exactMatch(app->arguments().at(1))){
        cout << "invalid gpio pin" << endl;
        return false;
    }
    this->gpioPinNumber = app->arguments().at(1);
    if(!HC_VALID_IO_STATES.exactMatch(app->arguments().at(2))){
        cout << "invalid state to observe" << endl;
        return false;
    }
    this->stateToObserve = app->arguments().at(2).toStdString();
    if(!QFile::exists(app->arguments().at(3))){
        cout << "can not find " << app->arguments().at(3).toStdString() << endl;
        return false;
    }
    this->externalCommand = app->arguments().at(3);
    if(app->arguments().count() >4)
        this->argumentList << app->arguments().at(4).split(" ");
    return true;
}

void MainClass::initGpio()
{
    this->gpioClass = std::unique_ptr<GPIOClass>(new GPIOClass(this->gpioPinNumber.toStdString()));
	this->gpioClass->setdir_gpio("in");
    cout << "setting GPIO pin direction 'in'" << endl;
	this->gpioClass->getval_gpio(this->lastInputState);
    cout << now() << "current input state is " << this->lastInputState << endl;
}

void MainClass::executeExternalProcess()
{
    cout << now() << "executing [" << this->externalCommand.toStdString() << " ";
	foreach (QString partialArgument, this->argumentList) {
		cout << partialArgument.toStdString() << " ";
	}
	cout << "] ... " << endl;
	// prevent calling executable more than once at the same time
    // TODO: check if we really need to do this
	this->observerTimer->stop();
	process->start(this->externalCommand, this->argumentList);
	process->waitForFinished();
	if(QProcess::NormalExit == process->exitStatus() &&
			0 == process->exitCode())
		cout << "OK" << endl;
	else
		cout << "ERR" << endl;
	// executed, continue surveillance
    this->observerTimer->start();
}

void MainClass::printCommandLineStandardOutput()
{
    QByteArray stdOutByteArray = this->process->readAllStandardOutput();
    if(stdOutByteArray.isNull() ||
            stdOutByteArray.isEmpty())
        return;
    cout << now() << QString(stdOutByteArray).toStdString() << endl;
}

void MainClass::printCommandLineErrorOutput()
{
    QByteArray errOutByteArray = this->process->readAllStandardOutput();
    if(errOutByteArray.isNull() ||
            errOutByteArray.isEmpty())
        return;
    cerr << now() << QString(errOutByteArray).toStdString() << endl;
}

string MainClass::now()
{
    return QDate::currentDate().toString("yyyyMMdd ").toStdString() + QTime::currentTime().toString("hh:mm:ss : ").toStdString();
}

void MainClass::aboutToQuitApp()
{
    cout << endl << "aboutToQuitApp....." << endl;
	if(this->observerTimer->isActive())
		this->observerTimer->stop();
	if(QProcess::Running||QProcess::Starting == this->process->state())
		this->process->kill();
	cout << "stopped" << endl;
}

void MainClass::fireTimer()
{
	std::string currentInputState;
	this->gpioClass->getval_gpio(currentInputState);
	if(this->lastInputState != currentInputState){
        cout << now() << "input value has changed, now in state " << currentInputState << endl;
		if(currentInputState == this->stateToObserve)
            executeExternalProcess();
		this->lastInputState = currentInputState;
	}
}

