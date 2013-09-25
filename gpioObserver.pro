#-------------------------------------------------
#
# Project created by QtCreator 2013-09-21T17:40:15
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET    = gpioObserver
CONFIG   += console
CONFIG   -= app_bundle
greaterThan(QT_MAJOR_VERSION, 4):{
  # this is just a way to be able to compile the code with OSX
  # using the Mac as development machine
  macx{
    CONFIG   += c++11
  }
  cache()
}
else{
  # on Raspberry Pi there is a gcc 4.6.3
  QMAKE_CXXFLAGS += -std=gnu++0x -Wextra -pedantic
}

TEMPLATE = app

makeinstall.path += /usr/local/bin
makeinstall.files = $$[QT_INSTALL_DATA]/$$[TARGET]
makeinstall.extra = "sudo chmod 700 /usr/local/bin/"$$[TARGET]
INSTALLS += makeinstall

SOURCES += main.cpp \
    mainclass.cpp \
    GPIOClass.cpp

HEADERS += \
    mainclass.h \
    GPIOClass.h
