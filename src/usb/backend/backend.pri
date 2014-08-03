INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += native_usb.h
SOURCES += native_usb.cpp

win32 {
    INCLUDEPATH += binary/win/libusb-1.0
    LIBS += binary/win/libusb-1.0/libusb-1.0.a
}

linux {
    INCLUDEPATH += /usr/local/include/libusb-1.0
    LIBS += /usr/local/lib/libusb-1.0.a -ludev
}

macx {
    include(osx/osx.pri)
}
