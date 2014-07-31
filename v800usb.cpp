#include "v800usb.h"

#include <QFile>
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <stdio.h>

#include "native_usb.h"

V800usb::V800usb(QObject *parent) :
    QObject(parent)
{
    usb = NULL;
}

V800usb::~V800usb()
{
    if(usb != NULL)
    {
        usb->close_usb();
        delete usb;
    }
}

void V800usb::start()
{
    usb = new native_usb();
    if(usb->open_usb(0x0da4, 0x0008) != -1)
    {
        get_all_sessions();
        emit ready();
    }
    else
    {
        emit not_ready();
    }
}

void V800usb::get_all_sessions()
{
    QList<QByteArray> dates, times, files, sessions;
    int dates_iter, times_iter, files_iter;
    bool session_exists = false;

    dates = get_all_dates();
    for(dates_iter = 0; dates_iter < dates.length(); dates_iter++)
    {
        times.clear();
        times = get_all_times(dates[dates_iter]);
        for(times_iter = 0; times_iter < times.length(); times_iter++)
        {
            files.clear();
            files = get_all_files(dates[dates_iter], times[times_iter]);
            for(files_iter = 0; files_iter < files.length(); files_iter++)
            {
                if(QString(files[files_iter]).compare("ROUTE.GZB") == 0)
                {
                    session_exists = true;
                    break;
                }
            }

            if(session_exists)
            {
                QString date(dates[dates_iter].constData());
                QString time(times[times_iter].constData());

                QString combined((QString("%1%2").arg(date).arg(time).toLatin1()));
                QDateTime session_time = QDateTime::fromString(combined, "yyyyMMdd/HHmmss/");

                sessions.append(QByteArray(session_time.toString(Qt::TextDate).toLatin1()));
                session_exists = false;
            }
        }
    }

    emit all_sessions(sessions);
}

void V800usb::get_session(QByteArray session, QString save_dir)
{
    QDateTime session_time = QDateTime::fromString(session.constData(), Qt::TextDate);
    QString session_str = session_time.toString("yyyyMMdd/HHmmss");
    QStringList session_split = session_str.split('/');
    QList<QByteArray> files;
    int files_iter;

    if(session_split.length() == 2)
    {
        files = get_all_files(session_split[0].toLatin1(), session_split[1].toLatin1());
        for(files_iter = 0; files_iter < files.length(); files_iter++)
            get_file(session_split[0].toLatin1(), session_split[1].toLatin1(), files[files_iter], SESSION_DATA, save_dir);
        get_session_info(session_split[0].toLatin1(), session_split[1].toLatin1(), save_dir);
    }

    emit session_done();
}

void V800usb::get_session_info(QByteArray date, QByteArray time, QString save_dir)
{
    get_file(date, time, QByteArray("TSESS.BPB"), SESSION_INFO, save_dir);
    get_file(date, time, QByteArray("PHYSDATA.BPB"), SESSION_INFO, save_dir);
}

QList<QByteArray> V800usb::get_all_dates()
{
    QList<QByteArray> dates;
    QByteArray packet, full;
    int cont = 1, usb_state = 0, packet_num = 0, initial_packet = 1;

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            packet.clear();
            packet = generate_request("/U/0/");

            usb->write_usb(packet);

            packet_num = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            packet.clear();
            packet = usb->read_usb();

            full = add_to_full(packet, full, initial_packet);

            // initial packet seems to always have two extra bytes in the front, 0x00 0x00
            if(initial_packet)
                initial_packet = 0;

            // check for end of buffer
            if(is_end(packet))
                usb_state = 4;
            else
                usb_state = 2;
            break;
        case 2: // send an ack packet
            packet.clear();
            packet = generate_ack(packet_num);
            if(packet_num == 0xff)
                packet_num = 0x00;
            else
                packet_num++;

            usb->write_usb(packet);

            usb_state = 1;
            break;
        case 4:
            qDebug("Done getting dates from V800");

            dates = extract_dir_and_files(full);

            cont = 0;
            break;
        }
    }

    return dates;
}

QList<QByteArray> V800usb::get_all_times(QByteArray date)
{
    QList<QByteArray> times;
    QByteArray packet, full;
    int cont = 1, usb_state = 0, packet_num = 0, initial_packet = 1;

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            packet.clear();
            packet = generate_request(QString("/U/0/%1/E/").arg(date.constData()));

            usb->write_usb(packet);

            packet_num = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            packet.clear();
            packet = usb->read_usb();

            full = add_to_full(packet, full, initial_packet);

            // initial packet seems to always have two extra bytes in the front, 0x00 0x00
            if(initial_packet)
                initial_packet = 0;

            // check for end of buffer
            if(is_end(packet))
                usb_state = 4;
            else
                usb_state = 2;
            break;
        case 2: // send an ack packet
            packet.clear();
            packet = generate_ack(packet_num);
            if(packet_num == 0xff)
                packet_num = 0x00;
            else
                packet_num++;

            usb->write_usb(packet);

            usb_state = 1;
            break;
        case 4:
            qDebug("Done getting times from V800");

            times = extract_dir_and_files(full);

            cont = 0;
            break;
        }
    }

    return times;
}

QList<QByteArray> V800usb::get_all_files(QByteArray date, QByteArray time)
{
    QList<QByteArray> files;
    QByteArray packet, full;
    int cont = 1, usb_state = 0, packet_num = 0, initial_packet = 1;

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            packet.clear();
            packet = generate_request(QString("/U/0/%1/E/%2/00/").arg(date.constData()).arg(time.constData()));

            usb->write_usb(packet);

            packet_num = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            packet.clear();
            packet = usb->read_usb();

            full = add_to_full(packet, full, initial_packet);

            // initial packet seems to always have two extra bytes in the front, 0x00 0x00
            if(initial_packet)
                initial_packet = 0;

            // check for end of buffer
            if(is_end(packet))
                usb_state = 4;
            else
                usb_state = 2;
            break;
        case 2: // send an ack packet
            packet.clear();
            packet = generate_ack(packet_num);
            if(packet_num == 0xff)
                packet_num = 0x00;
            else
                packet_num++;

            usb->write_usb(packet);

            usb_state = 1;
            break;
        case 4:
            qDebug("Done getting files from V800");

            files = extract_dir_and_files(full);

            cont = 0;
            break;
        }
    }

    return files;
}

void V800usb::get_file(QByteArray date, QByteArray time, QByteArray file, int type, QString save_dir)
{   
    QFile *out_file;
    QByteArray packet, full;
    int cont = 1, usb_state = 0, packet_num = 0, initial_packet = 1;

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            packet.clear();
            if(type == SESSION_DATA)
                packet = generate_request(QString("/U/0/%1/E/%2/00/%3").arg(date.constData()).arg(time.constData()).arg(file.constData()));
            else if(type == SESSION_INFO)
                packet = generate_request(QString("/U/0/%1/E/%2/%3").arg(date.constData()).arg(time.constData()).arg(file.constData()));

            usb->write_usb(packet);

            packet_num = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            packet.clear();
            packet = usb->read_usb();

            full = add_to_full(packet, full, initial_packet);

            // initial packet seems to always have two extra bytes in the front, 0x00 0x00
            if(initial_packet)
                initial_packet = 0;

            // check for end of buffer
            if(is_end(packet))
                usb_state = 4;
            else
                usb_state = 2;
            break;
        case 2: // send an ack packet
            packet.clear();
            packet = generate_ack(packet_num);
            if(packet_num == 0xff)
                packet_num = 0x00;
            else
                packet_num++;

            usb->write_usb(packet);

            usb_state = 1;
            break;
        case 4:
#if defined(Q_OS_WIN)
            QString session_path = (QString("%1\\%2\\%3").arg(save_dir).arg(date.constData())).arg(time.constData());
            QString session_full_name = (QString("%1\\%2\\%3\\%4").arg(save_dir).arg(date.constData()).arg(time.constData()).arg(file.constData()));
#endif
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
            QString session_path = (QString("%1/%2/%3").arg(save_dir).arg(date.constData())).arg(time.constData());
            QString session_full_name = (QString("%1/%2/%3/%4").arg(save_dir).arg(date.constData()).arg(time.constData()).arg(file.constData()));
#endif
            QDir session_dir = QDir(session_path);

            if(!session_dir.exists())
                session_dir.mkpath(".");

            out_file = new QFile(session_full_name);
            out_file->open(QIODevice::WriteOnly);
            out_file->write(full);
            out_file->close();

            qDebug("Done getting file from V800");

            cont = 0;
            break;
        }
    }

}

QList<QByteArray> V800usb::extract_dir_and_files(QByteArray full)
{
    QList<QByteArray> dir_and_files;
    int full_state = 0, size = 0, loc = 0;

    while(loc < full.length())
    {
        switch(full_state)
        {
        case 0: /* look for 0x0A */
            if(full[loc] == (char)0x0A)
            {
                loc++;
                full_state = 1;
            }

            loc++;
            break;
        case 1: /* is this the second 0x0A? */
            if(full[loc] == (char)0x0A)
                full_state = 2;
            else
                full_state = 0;

            loc++;
            break;
        case 2: /* get the size */
            size = full[loc];

            full_state = 3;
            loc++;
            break;
        case 3: /* now get the full string */
            dir_and_files.append(QByteArray(full.constData()+loc, size));

            full_state = 0;
            loc += size;
            break;
        }
    }

    return dir_and_files;
}

QByteArray V800usb::generate_request(QString request)
{
    QByteArray packet;

    packet[0] = 01;
    packet[1] = (unsigned char)((request.length()+8) << 2);
    packet[2] = 0x00;
    packet[3] = request.length()+4;
    packet[4] = 0x00;
    packet[5] = 0x08;
    packet[6] = 0x00;
    packet[7] = 0x12;
    packet[8] = request.length();
    packet.append(request);

    return packet;
}

QByteArray V800usb::generate_ack(unsigned char packet_num)
{
    QByteArray packet;

    packet[0] = 0x01;
    packet[1] = 0x05;
    packet[2] = packet_num;

    return packet;
}

int V800usb::is_end(QByteArray packet)
{
    if((packet[1] & 0x03) == 1)
        return 0;
    else
        return 1;
}

QByteArray V800usb::add_to_full(QByteArray packet, QByteArray full, int initial_packet)
{
    QByteArray new_full = full;
    unsigned int size = packet[1] >> 2;

    if(initial_packet)
    {
        size -= 3;
        packet.remove(0, 5);
        new_full.append(packet);
    }
    else
    {
        size -= 1;
        packet.remove(0, 3);
        new_full.append(packet);
    }

    return new_full;
}
