#include "mainclass.h"
#include <stdio.h>
#include <QDate>

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
	parseCommandLine();
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
	cout << "gpioObserver, (c)2013 by Sven Putze" << endl;
	cout << "usage:" << std::endl << endl;
	cout << "gpioObserver <GPIO pin number> <state to observe> <executable> <arguments>" << endl;
}

void MainClass::parseCommandLine()
{
	if(5!=(app->arguments().count())){
		usage();
		finishWithError(1);
	}
	else{
		this->gpioPinNumber = app->arguments().at(1);
		this->stateToObserve = app->arguments().at(2).toStdString();
		cout << "waiting for next change to state " << this->stateToObserve << endl;
		this->externalCommand = app->arguments().at(3);
		this->argumentList << app->arguments().at(4).split(" ");
	}
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
    return QDate::currentDate().toString("YYYYMMDD-hh:mm:ss - ").toStdString();
}

void MainClass::aboutToQuitApp()
{
    cout << endl << now() << "aboutToQuitApp....." << endl;
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

