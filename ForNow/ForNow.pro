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
    Analyzer.cpp \
    TimeSeriesDatabase.cpp \
    view.cpp

HEADERS += \
    Analyzer.h \
    TimeSeriesDatabase.h \
    view.h

FORMS +=
