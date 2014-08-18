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

#include "native_usb.h"
#include <QByteArray>
#include <unistd.h>

#if defined(Q_OS_MAC)
extern "C"
{
    int rawhid_open(int max, int vid, int pid, int usage_page, int usage);
    int rawhid_recv(int num, void *buf, int len, int timeout);
    int rawhid_send(int num, void *buf, int len, int timeout);
    void rawhid_close(int num);
}
#endif

native_usb::native_usb(QObject *parent) :
    QObject(parent)
{
}

int native_usb::open_usb(int vid, int pid)
{
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    int r;
    usb = NULL;

    r = libusb_init(NULL);
    if (r < 0)
        return -1;

    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_INFO);

    qDebug("Opening the USB device...");
    usb = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if(usb == NULL)
    {
        qDebug("Error opening device");
        return -1;
    }

#if defined(Q_OS_LINUX)
    r = libusb_detach_kernel_driver(usb, 0);
    if(r != 0)
    {
        qDebug("Detach Kernel Driver: %s", libusb_error_name(r));
    }
#endif

    r = libusb_claim_interface(usb, 0);
    if(r != 0)
    {
        qDebug("Claim Interface: %s", libusb_error_name(r));
        return -1;
    }

    qDebug("Releasing interface and closing the USB device...");
    libusb_release_interface(usb, 0);
    libusb_close(usb);
    usb = NULL;

#if defined(Q_OS_WIN)
    Sleep(500);
#endif
#if defined(Q_OS_LINUX)
    usleep(500000);
#endif

    qDebug("Reopening the USB device...");
    usb = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if(usb == NULL)
    {
        qDebug("Error opening device");
        return -1;
    }

    r = libusb_claim_interface(usb, 0);
    if(r != 0)
    {
        qDebug("Reclaim Interface: %s", libusb_error_name(r));
        return -1;
    }

#if defined(Q_OS_WIN)
    Sleep(500);
#endif
#if defined(Q_OS_LINUX)
    usleep(500000);
#endif

    return 0;
#endif
#if defined(Q_OS_MAC)
    int r;

    r = rawhid_open(1, vid, pid, 0, 0);
    if(r == 0)
    {
        usb = -1;
        return -1;
    }

    usb = 0;
    return 0;
#endif
}

int native_usb::write_usb(QByteArray packet)
{
    QByteArray correct_packet;
    int actual_length;

#if defined(Q_OS_MAC)
    if(usb == -1)
        return -1;
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    if(usb == NULL)
        return -1;
#endif

    if(packet.length() > 64)
        return -1;

    correct_packet.resize(64 - packet.length());
    correct_packet.fill(0x00);
    correct_packet.prepend(packet);

#if defined(Q_OS_MAC)
    actual_length = rawhid_send(usb, (void *)correct_packet.constData(), correct_packet.length(), 1000);
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    libusb_interrupt_transfer(usb, (1 | LIBUSB_ENDPOINT_OUT), (unsigned char *)correct_packet.constData(), correct_packet.length(), &actual_length, 0);
#endif

    return actual_length;
}

QByteArray native_usb::read_usb()
{
    QByteArray packet;
    unsigned char char_packet[64];
    int actual_length;

#if defined(Q_OS_MAC)
    if(usb == -1)
        return packet;
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    if(usb == NULL)
        return packet;
#endif

    memset(char_packet, 0x00, sizeof(char_packet));
#if defined(Q_OS_MAC)
    actual_length = rawhid_recv(usb, char_packet, sizeof(char_packet), 1000);
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    libusb_interrupt_transfer(usb, (1 | LIBUSB_ENDPOINT_IN), char_packet, sizeof(char_packet), &actual_length, 0);
#endif
    packet.append((char *)char_packet, actual_length);

    return packet;
}

int native_usb::close_usb()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    int r;

    if(usb == NULL)
        return -1;

    r = libusb_release_interface(usb, 0);
    if(r != 0)
    {
        qDebug("Release interface: %s", libusb_error_name(r));
        return -1;
    }

#if defined(Q_OS_LINUX)
    libusb_attach_kernel_driver(usb, 0);
#endif

    libusb_close(usb);
    libusb_exit(NULL);

   return 0;
#endif
#if defined(Q_OS_MAC)
    if(usb == -1)
        return -1;

   rawhid_close(usb);

   return 0;
#endif
}
