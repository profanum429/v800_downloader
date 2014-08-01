#-------------------------------------------------
#
# Project created by QtCreator 2014-07-28T15:50:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = v800_downloader
TEMPLATE = app


SOURCES += main.cpp\
        v800main.cpp \
    v800usb.cpp \
    native_usb.cpp

macx {
    SOURCES += rawhid_api.c
}

HEADERS  += v800main.h \
    v800usb.h \
    native_usb.h

FORMS    += v800main.ui

CONFIG(debug,debug|release) DESTDIR = debug
CONFIG(release,debug|release) DESTDIR = release
MOC_DIR = $$DESTDIR/tmp
OBJECTS_DIR = $$DESTDIR/tmp
RCC_DIR = $$DESTDIR/tmp
UI_DIR = $$DESTDIR/tmp

osx:LIBS += -framework CoreFoundation -framework IOKit
win32:INCLUDEPATH += C:/Qt/libusb-1.0
win32:LIBS += C:/Qt/libusb-1.0/libusb-1.0.a
