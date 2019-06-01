#-------------------------------------------------
#
# Project created by QtCreator 2018-10-09T13:38:09
#
#-------------------------------------------------

QT += serialport
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JS_Robot_UGV
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += D:/OpenCV3.4.3/opencv_3.4.3_build_qt/install/include\
               D:/OpenCV3.4.3/opencv_3.4.3_build_qt/install/include/opencv\
               D:/OpenCV3.4.3/opencv_3.4.3_build_qt/install/include/opencv2


LIBS += D:/OpenCV3.4.3/opencv_3.4.3_build_qt/install/x86/mingw/lib/libopencv_*.dll.a

FORMS += \
    code/mainwindow.ui

DISTFILES += \
    params.xml

HEADERS += \
    code/camera.h \
    code/common.h \
    code/control.h \
    code/detector.h \
    code/mainwindow.h \
    code/serial.h

SOURCES += \
    code/camera.cpp \
    code/control.cpp \
    code/detector.cpp \
    code/main.cpp \
    code/mainwindow.cpp \
    code/serial.cpp
