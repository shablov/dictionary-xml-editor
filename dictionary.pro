#-------------------------------------------------
#
# Project created by QtCreator 2015-02-01T01:00:48
#
#-------------------------------------------------

QT       += core gui xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = bin/dictionary

MOC_DIR = obj
OBJECTS_DIR = obj
RCC_DIR = obj

SOURCES += main.cpp\
        mainwindow.cpp \
    dictionarymodel.cpp \
    dictionaryitem.cpp

HEADERS  += mainwindow.h \
    dictionarymodel.h \
    dictionaryitem.h

RESOURCES += \
    resources.qrc
