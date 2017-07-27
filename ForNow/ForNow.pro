#-------------------------------------------------
#
# Project created by QtCreator 2017-07-18T22:30:33
#
#-------------------------------------------------

QT       += core gui sql testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ForNow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    database.cpp \
    analize.cpp \
    Analyzer.cpp \
    TimeSeriesDatabase.cpp

HEADERS += \
        mainwindow.h \
    database.h \
    analize.h \
    Analyzer.h \
    TimeSeriesDatabase.h

FORMS += \
        mainwindow.ui
