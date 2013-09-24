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
  CONFIG   += c++11
  cache()
}
else{
  QMAKE_CXXFLAGS += -std=gnu++0x -Wextra -pedantic
}

TEMPLATE = app


SOURCES += main.cpp \
    mainclass.cpp \
    GPIOClass.cpp

HEADERS += \
    mainclass.h \
    GPIOClass.h
