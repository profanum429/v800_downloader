/*
    Copyright 2014 Christian Weber

    This file is part of V800 Downloader.

    V800 Downloader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    V800 Downloader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with V800 Downloader.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#if defined(Q_OS_MAC)
    int usb;
#endif
};

#endif // NATIVE_USB_H
