#-------------------------------------------------
#
# Project created by QtCreator 2017-07-18T22:30:33
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ForNow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    database.cpp

HEADERS += \
        mainwindow.h \
    database.h

FORMS += \
        mainwindow.ui
