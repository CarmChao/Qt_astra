QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/include/eigen3
INCLUDEPATH += /usr/include/boost

LIBS += /usr/local/lib/libopencv*.so.3.4 \
        -lboost_system

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    opencvthread.cpp \
    openni_driver.cpp \
    uvc_driver.cpp

HEADERS += \
    mainwindow.h \
    opencvthread.h \
    openni_driver.h \
    uvc_driver.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


unix:!macx: LIBS += -L$$PWD/../../../../../opt/ros/melodic/lib/x86_64-linux-gnu/ -luvc

INCLUDEPATH += $$PWD/../../../../../opt/ros/melodic/include
DEPENDPATH += $$PWD/../../../../../opt/ros/melodic/include

unix:!macx: LIBS += -L$$PWD/Redist/ -lOpenNI2

INCLUDEPATH += $$PWD/Include
DEPENDPATH += $$PWD/Include
