#-------------------------------------------------
#
# Project created by QtCreator 2016-01-26T14:59:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PiController
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    curlthread.cpp

HEADERS  += mainwindow.h \
    curlthread.h

FORMS    += mainwindow.ui

LIBS    += -L/usr/local/lib \
           -lopencv_core \
           -lopencv_highgui \
           -lopencv_videoio \
           -lopencv_imgproc \
           -lopencv_imgcodecs \
           -lcurl
