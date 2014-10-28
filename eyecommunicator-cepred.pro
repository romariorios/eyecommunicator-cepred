#-------------------------------------------------
#
# Project created by QtCreator 2014-10-08T02:50:22
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eyecommunicator-cepred
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += \
    mainwindow.hpp \
    baseeyetrackerplugin.hpp \
    eyesposition.hpp \
    basetrackingcalibrationwidget.hpp

FORMS    += mainwindow.ui
