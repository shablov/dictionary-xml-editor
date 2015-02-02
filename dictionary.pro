#-------------------------------------------------
#
# Project created by QtCreator 2015-02-01T01:00:48
#
#-------------------------------------------------

QT       += core gui xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5_2): DEFINES += QT_5_2

TEMPLATE = app
TARGET = bin/dictionary

MOC_DIR = obj
OBJECTS_DIR = obj
RCC_DIR = obj

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

RESOURCES += \
    resources.qrc
