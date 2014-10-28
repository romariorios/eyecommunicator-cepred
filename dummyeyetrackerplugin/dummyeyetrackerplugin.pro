#-------------------------------------------------
#
# Project created by QtCreator 2014-10-08T04:15:02
#
#-------------------------------------------------

CONFIG += c++11

TARGET = dummyeyetrackerplugin
TEMPLATE = lib
CONFIG += staticlib

SOURCES += dummyeyetrackerplugin.cpp \
    dummytrackingcalibrationwidget.cpp

HEADERS += dummyeyetrackerplugin.hpp \
    dummytrackingcalibrationwidget.hpp
unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    dummytrackingcalibrationwidget.ui
