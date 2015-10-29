#-------------------------------------------------
#
# Project created by QtCreator 2015-10-29T18:02:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    drawbase.cpp \
    cpudraw.cpp

HEADERS  += mainwindow.h \
    drawbase.h \
    cpudraw.h

FORMS    += mainwindow.ui

LDFLAGS += std=c++11
