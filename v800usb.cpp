#include "v800usb.h"

#include <QFile>
#include <QDateTime>
#include <QStringList>
#include <QDir>
#include <stdio.h>

V800usb::V800usb(QObject *parent) :
    QObject(parent)
{
}

V800usb::~V800usb()
{
    int r;

    if(v800 != NULL)
    {
        r = libusb_release_interface(v800, 0);
        qDebug("Release interface: %s", libusb_error_name(r));

        libusb_close(v800);
        libusb_exit(NULL);
    }
}

void V800usb::loop()
{
    int r;

    v800 = NULL;

    r = libusb_init(NULL);
    if (r < 0)
        return;

    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_INFO);

    qDebug("Opening the V800...");
    v800 = libusb_open_device_with_vid_pid(NULL, 0x0da4, 0x0008);
    if(v800 == NULL)
    {
        qDebug("Error opening device");
        emit not_ready();

        return;
    }

    r = libusb_claim_interface(v800, 0);
    qDebug("Claim Interface: %s", libusb_error_name(r));

    qDebug("Releasing interface and closing V800...");
    libusb_release_interface(v800, 0);
    libusb_close(v800);
    v800 = NULL;

    Sleep(500);

    qDebug("Reopening the V800...");
    v800 = libusb_open_device_with_vid_pid(NULL, 0x0da4, 0x0008);
    if(v800 == NULL)
    {
        qDebug("Error opening device");
        emit not_ready();

        return;
    }

    r = libusb_claim_interface(v800, 0);
    qDebug("Reclaim Interface: %s", libusb_error_name(r));

    Sleep(500);

    get_all_sessions();
    emit ready();
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

void V800usb::get_session(QByteArray session)
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
            get_file(session_split[0].toLatin1(), session_split[1].toLatin1(), files[files_iter]);
        get_session_info(session_split[0].toLatin1(), session_split[1].toLatin1());
    }

    emit session_done();
}

QList<QByteArray> V800usb::get_all_dates()
{
    QList<QByteArray> dates;
    unsigned char char_dates[512][64];
    int dates_cnt, dates_iter;

    if(v800 != NULL)
    {
        dates_cnt = get_all_dates(v800, char_dates);

        for(dates_iter = 0; dates_iter < dates_cnt; dates_iter++)
            dates.append(QByteArray((char *)char_dates[dates_iter]));
    }

    return dates;
}

QList<QByteArray> V800usb::get_all_times(QByteArray date)
{
    QList<QByteArray> times;
    unsigned char char_times[512][64];
    int times_cnt, times_iter;

    if(v800 != NULL)
    {
        times_cnt = get_all_times(v800, (unsigned char *)date.constData(), char_times);

        for(times_iter = 0; times_iter < times_cnt; times_iter++)
            times.append(QByteArray((char *)char_times[times_iter]));
    }

    return times;
}

QList<QByteArray> V800usb::get_all_files(QByteArray date, QByteArray time)
{
    QList<QByteArray> files;
    unsigned char char_files[512][64];
    char dir[64];
    int files_cnt, files_iter;

    if(v800 != NULL)
    {
        snprintf(dir, sizeof(dir), "/U/0/%s/E/%s/00/", date.constData(), time.constData());
        files_cnt = get_all_files(v800, dir, char_files);

        for(files_iter = 0; files_iter < files_cnt; files_iter++)
            files.append(QByteArray((char *)char_files[files_iter]));
    }

    return files;
}

void V800usb::get_file(QByteArray date, QByteArray time, QByteArray file)
{
    char dir[64];

    if(v800 != NULL)
    {
        snprintf(dir, sizeof(dir), "/U/0/%s/E/%s/00/", date.constData(), time.constData());
        get_file(v800, dir, (unsigned char *)file.constData());
    }
}

void V800usb::get_session_info(QByteArray date, QByteArray time)
{
    char dir[64];

    if(v800 != NULL)
    {
        snprintf(dir, sizeof(dir), "/U/0/%s/E/%s/", date.constData(), time.constData());
        get_file(v800, dir, (unsigned char *)"TSESS.BPB");
        get_file(v800, dir, (unsigned char *)"PHYSDATA.BPB");
    }
}

int V800usb::extract_dir_and_files(unsigned char *full, int full_size, unsigned char dir_and_files[][64])
{
    int full_state = 0, size = 0, loc = 0, str_loc = 0;

    while(loc < full_size)
    {
        switch(full_state)
        {
        case 0: /* look for 0x0A */
            if(full[loc] == 0x0A)
            {
                loc++;
                full_state = 1;
            }

            loc++;
            break;
        case 1: /* is this the second 0x0A? */
            if(full[loc] == 0x0A)
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
            memcpy(dir_and_files[str_loc], full+loc, size);
            str_loc++;

            full_state = 0;
            loc += size;
            break;
        }
    }

    return str_loc;
}

void V800usb::generate_request(char *request, unsigned char *packet)
{
    packet[0] = 01;
    packet[1] = (unsigned char)((strlen(request)+8) << 2);
    packet[2] = 0x00;
    packet[3] = strlen(request)+4;
    packet[4] = 0x00;
    packet[5] = 0x08;
    packet[6] = 0x00;
    packet[7] = 0x12;
    packet[8] = strlen(request);
    memcpy(packet+9, request, strlen(request));
}

void V800usb::generate_ack(unsigned char packet_num, unsigned char *packet)
{
    packet[0] = 0x01;
    packet[1] = 0x05;
    packet[2] = packet_num;
}

int V800usb::is_end(unsigned char *packet)
{
    if((packet[1] & 0x03) == 1)
        return 0;
    else
        return 1;
}

int V800usb::add_to_full(unsigned char *packet, unsigned char *full, int full_size, int initial_packet)
{
    unsigned int size = packet[1] >> 2;

    if(initial_packet)
    {
        size -= 3;
        memcpy(full+full_size, packet+5, size);
    }
    else
    {
        size -= 1;
        memcpy(full+full_size, packet+3, size);
    }

    return full_size+size;
}

int V800usb::get_all_dates(libusb_device_handle *v800, unsigned char dates[][64])
{
    unsigned char packet[64], full[64*512], dir_and_files[256][64];
    int r, actual_length, cont = 1, usb_state = 0, packet_num = 0, full_size = 0, dir_and_files_cnt = 0, initial_packet = 1;

    memset(dir_and_files, 0x00, sizeof(dir_and_files));

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            memset(packet, 0x00, sizeof(packet));
            generate_request((char *)"/U/0/", packet);

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Request to V800: %s", libusb_error_name(r));

            packet_num = 0;
            memset(full, 0x00, sizeof(full));
            full_size = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            memset(packet, 0x00, sizeof(packet));

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_IN), packet, sizeof(packet), &actual_length, 0);
//            qDebug("V800 Reponse: %s", libusb_error_name(r));

            full_size = add_to_full(packet, full, full_size, initial_packet);

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
            memset(packet, 0x00, sizeof(packet));
            generate_ack(packet_num++, packet);

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Ack Packet: %s", libusb_error_name(r));

            usb_state = 1;
            break;
        case 4:
            qDebug("Done getting dates from V800");

            dir_and_files_cnt = extract_dir_and_files(full, full_size, dir_and_files);
            memcpy(dates, dir_and_files, sizeof(dir_and_files));

            cont = 0;
            break;
        }
    }

    return dir_and_files_cnt;
}

int V800usb::get_all_times(libusb_device_handle *v800, unsigned char *date, unsigned char times[][64])
{
    char request[64];
    unsigned char packet[64], full[64*512], dir_and_files[256][64];
    int r, actual_length, cont = 1, usb_state = 0, packet_num = 0, full_size = 0, dir_and_files_cnt = 0, initial_packet = 1;

    memset(dir_and_files, 0x00, sizeof(dir_and_files));

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            memset(packet, 0x00, sizeof(packet));
            snprintf(request, sizeof(request), "/U/0/%s/E/", date);
            generate_request(request, packet);

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Request to V800: %s", libusb_error_name(r));

            packet_num = 0;
            memset(full, 0x00, sizeof(full));
            full_size = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            memset(packet, 0x00, sizeof(packet));

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_IN), packet, sizeof(packet), &actual_length, 0);
//            qDebug("V800 Reponse: %s", libusb_error_name(r));

            full_size = add_to_full(packet, full, full_size, initial_packet);

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
            memset(packet, 0x00, sizeof(packet));
            generate_ack(packet_num, packet);
            if(packet_num == 0xff)
                packet_num = 0x00;
            else
                packet_num++;

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Ack Packet: %s", libusb_error_name(r));

            usb_state = 1;
            break;
        case 4:
            qDebug("Done getting times from V800");

            dir_and_files_cnt = extract_dir_and_files(full, full_size, dir_and_files);
            memcpy(times, dir_and_files, sizeof(dir_and_files));

            cont = 0;
            break;
        }
    }

    return dir_and_files_cnt;
}

int V800usb::get_all_files(libusb_device_handle *v800, char *dir, unsigned char files[][64])
{
    char request[64];
    unsigned char packet[64], full[64*512], dir_and_files[256][64];
    int r, actual_length, cont = 1, usb_state = 0, packet_num = 0, full_size = 0, dir_and_files_cnt = 0, initial_packet = 1;

    memset(dir_and_files, 0x00, sizeof(dir_and_files));

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            memset(packet, 0x00, sizeof(packet));
            snprintf(request, sizeof(request), "%s", dir);
            generate_request(request, packet);

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Request to V800: %s", libusb_error_name(r));

            packet_num = 0;
            memset(full, 0x00, sizeof(full));
            full_size = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            memset(packet, 0x00, sizeof(packet));

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_IN), packet, sizeof(packet), &actual_length, 0);
//            qDebug("V800 Reponse: %s", libusb_error_name(r));

            full_size = add_to_full(packet, full, full_size, initial_packet);

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
            memset(packet, 0x00, sizeof(packet));
            generate_ack(packet_num++, packet);

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Ack Packet: %s", libusb_error_name(r));

            usb_state = 1;
            break;
        case 4:
            qDebug("Done getting file names from V800");

            dir_and_files_cnt = extract_dir_and_files(full, full_size, dir_and_files);
            memcpy(files, dir_and_files, sizeof(dir_and_files));

            cont = 0;
            break;
        }
    }

    return dir_and_files_cnt;
}

void V800usb::get_file(libusb_device_handle *v800, char *dir, unsigned char *file)
{
    char request[64];
    unsigned char packet[64], full[512*512];
    int r, actual_length, cont = 1, usb_state = 0, packet_num = 0, full_size = 0, initial_packet = 1;

    QFile *out_file;

    while(cont)
    {
        // usb state machine for reading
        switch(usb_state)
        {
        case 0: // send a command to the watch
            memset(packet, 0x00, sizeof(packet));
            snprintf(request, sizeof(request), "%s/%s", dir, file);
            generate_request(request, packet);

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Request to V800: %s", libusb_error_name(r));

            packet_num = 0;
            memset(full, 0x00, sizeof(full));
            full_size = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            memset(packet, 0x00, sizeof(packet));

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_IN), packet, sizeof(packet), &actual_length, 0);
//            qDebug("V800 Reponse: %s", libusb_error_name(r));

            full_size = add_to_full(packet, full, full_size, initial_packet);

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
            memset(packet, 0x00, sizeof(packet));
            generate_ack(packet_num++, packet);

            r = libusb_interrupt_transfer(v800, (1 | LIBUSB_ENDPOINT_OUT), packet, sizeof(packet), &actual_length, 0);
//            qDebug("Ack Packet: %s", libusb_error_name(r));

            usb_state = 1;
            break;
        case 4:            
            QStringList file_name_split = QString(dir).split('/');

            if(file_name_split.length() >= 5)
            {
                QString session_path = (QString("%1\\%2").arg(file_name_split[3])).arg(file_name_split[5]);
                QString session_full_name = (QString("%1\\%2\\%3").arg(file_name_split[3]).arg(file_name_split[5]).arg(QString((char *)file)));
                QDir session_dir = QDir(session_path);

                if(!session_dir.exists())
                    session_dir.mkpath(".");

                out_file = new QFile(session_full_name);
                out_file->open(QIODevice::WriteOnly);
                out_file->write((char *)full, full_size);
                out_file->close();
            }

            qDebug("Done getting file from V800");

            cont = 0;
            break;
        }
    }
}
