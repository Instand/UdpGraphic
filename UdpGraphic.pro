#-------------------------------------------------
#
# Project created by QtCreator 2016-08-23T15:37:49
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = UdpGraphic
TEMPLATE = app


SOURCES += main.cpp\
        basegui.cpp \
    qcustomplot.cpp \
    udpsettings.cpp \
    log.cpp

HEADERS  += basegui.h \
    qcustomplot.h \
    udpsettings.h \
    log.h

FORMS    += basegui.ui

RESOURCES += \
    res.qrc
