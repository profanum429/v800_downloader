#ifndef NATIVE_USB_H
#define NATIVE_USB_H

#include <QObject>
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    #include "libusb.h"
#endif

class native_usb : public QObject
{
    Q_OBJECT
public:
    explicit native_usb(QObject *parent = 0);
    int open_usb(int vid, int pid);
    int write_usb(QByteArray packet);
    QByteArray read_usb();
    int close_usb();

signals:

public slots:

private:
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    libusb_device_handle *usb;
#endif
};

#endif // NATIVE_USB_H
