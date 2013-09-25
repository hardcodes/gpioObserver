#include "mainclass.h"
#include <stdio.h>
#include <QDate>
#include <QFile>
#include <QTime>

using namespace std;

MainClass::MainClass(QObject *parent) :
	QObject(parent)
{
	app = QCoreApplication::instance();
	this->process = new QProcess(this);
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
    if(!parseCommandLine())
        return;
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
    cout << "gpioObserver v" << HC_PROGRAMVERSION << ", (c)2013 by Sven Putze" << endl << endl;
    cout << "  wait for one gpio pin to reach a desired state and execute a program afterwards" << endl << endl;
    cout << "  git commit hash: " << HC_GITHASH << endl;
	cout << "usage:" << std::endl << endl;
    cout << "  gpioObserver <GPIO pin number> <state to observe> <executable> [<arguments>]" << endl;
    cout << "  <GPIO pin number> must be one out of (17|21|22|23|24|25|27)" << endl;
    cout << "       21 is board revision 1" << endl;
    cout << "       27 is board revision 2 of the same pin" << endl;
    cout << "  <state to observe> must be one out of (0|1)" << endl;
    cout << "  <executable> is the file that should be executed if <state to observe> is reached" << endl;
    cout << "  [<arguments>] are optional commandline parameters for <executable>" << endl;
    cout << "       don't forget quotes if there is more then one parameter" << endl << endl;
}

bool MainClass::parseCommandLine()
{
    if(4<(app->arguments().count()) ||
            6>(app->arguments().count()) ||
            !parseAndValidateCommandLine())
    {
		usage();
		finishWithError(1);
        return false;
	}
    return true;
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
    if(5 == app->arguments().count())
        this->argumentList << app->arguments().at(4).split(" ");
    return true;
}

void MainClass::initGpio()
{
    this->gpioClass = std::unique_ptr<GPIOClass>(new GPIOClass(this->gpioPinNumber.toStdString()));
	this->gpioClass->setdir_gpio("in");
	cout << "set GPIO pin direction 'in'" << endl;
	this->gpioClass->getval_gpio(this->lastInputState);
    cout << now() << "current input state is " << this->lastInputState << endl;
}

void MainClass::executeCommandLine()
{
    cout << now() << "executing [" << this->externalCommand.toStdString() << " ";
	foreach (QString partialArgument, this->argumentList) {
		cout << partialArgument.toStdString() << " ";
	}
	cout << "] ... " << endl;
	// prevent calling executable more than once at the same time
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
			executeCommandLine();
		this->lastInputState = currentInputState;
	}
}

