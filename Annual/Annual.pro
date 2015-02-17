#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T12:18:04
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Annual
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    candidatalist.cpp \
    about.cpp \
    dbopr.cpp \
    model.cpp

HEADERS  += mainwindow.h \
    candidatalist.h \
    about.h \
    dbopr.h \
    model.h

FORMS    += mainwindow.ui \
    candidatalist.ui \
    about.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    resource.qrc
