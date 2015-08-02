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
    dictionaryitem.cpp \
    headerview.cpp \
    extreeview.cpp \
    datachangedcommand.cpp \
    insertitemcommand.cpp \
    removeitemcommand.cpp \
    itemcommand.cpp \
    updownitemcommand.cpp \
    cutpasteitemcommand.cpp

HEADERS  += mainwindow.h \
    dictionarymodel.h \
    dictionaryitem.h \
    headerview.h \
    extreeview.h \
    datachangedcommand.h \
    insertitemcommand.h \
    removeitemcommand.h \
    itemcommand.h \
    updownitemcommand.h \
    cutpasteitemcommand.h

RESOURCES += \
    resources.qrc

TRANSLATIONS += files/dictionary_ru.ts
