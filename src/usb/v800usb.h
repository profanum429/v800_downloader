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

#ifndef V800USB_H
#define V800USB_H

#include <QObject>

#define V800_ROOT_DIR   "/U/0"

enum {
    V800 = 0,
    M400
};

class native_usb;

class V800usb : public QObject
{
    Q_OBJECT
public:
    explicit V800usb(int device, QObject *parent = 0);
    ~V800usb();

signals:
    void all_sessions(QList<QString> sessions);
    void session_done(QString session, int session_iter, int session_cnt);
    void sessions_done();
    void ready();
    void not_ready();

    void all_objects(QList<QString> objects);
    void file_done();

public slots:
    void start();
    void get_sessions(QList<QString> sessions);

    void get_all_objects(QString path);
    void get_file(QString path);

    void upload_route(QString route);

private:
    QList<QString> extract_dir_and_files(QByteArray full);
    QByteArray generate_request(QString request);
    QByteArray generate_ack(unsigned char packet_num);
    int is_end(QByteArray packet);
    QByteArray add_to_full(QByteArray packet, QByteArray full, bool initial_packet, bool final_packet);

    QList<QString> get_v800_data(QString request, int multi_sport = 0, bool debug=false);

    void remove_v800_dir(QString dest);
    void put_v800_dir(QString dest);
    void put_v800_data(QString src, QString dest);
    QByteArray generate_directory_command(QByteArray dest, bool remove);
    QByteArray generate_initial_command(QByteArray dest, QByteArray data);
    QByteArray generate_next_command(QByteArray data, int packet_num);
    int is_command_end(QByteArray packet);

    void get_all_sessions();

    native_usb *usb;

    int device;
};

#endif // V800USB_H
