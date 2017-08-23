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
DEFINES += QT_FORCE_ASSERTS

SOURCES += \
        main.cpp \
    Analyzer.cpp \
    view.cpp \
    TimeSeriesDBI.cpp \
    TimeSeries.cpp \
    DataInMemmoryMoc.cpp \
    TestsPersonal.cpp \
    TTimeSeriesDBI.cpp

HEADERS += \
    Analyzer.h \
    view.h \
    TimeSeriesDBI.h \
    TimeSeries.h \
    DataInMemmoryMoc.h \
    TestsPersonal.h \
    TTimeSeriesDBI.h

FORMS +=

