#-------------------------------------------------
#
# Project created by QtCreator 2016-01-26T14:59:06
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PiController
TEMPLATE = app
CONFIG += c++11

# optimization flag ==> -O2
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_LFLAGS_RELEASE += -O2

SOURCES += main.cpp\
        mainwindow.cpp \
    curlthread.cpp \
    socketthread.cpp \
    protobuf/MsgChunk.pb.cc \
    msgassembler.cpp \
    MotorController/motorcontroller.cpp \

HEADERS  += mainwindow.h \
    curlthread.h \
    socketthread.h \
    protobuf/MsgChunk.pb.h \
    msgassembler.h \
    MotorController/motorcontroller.h \

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_features2d \
        -lopencv_highgui \
        -lopencv_video \
        -lopencv_tracking \
        -lopencv_videoio \
        -lopencv_imgcodecs \
        -lprotobuf \
        -lcurl




