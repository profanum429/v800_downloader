#ifndef V800USB_H
#define V800USB_H

#include <QObject>
#include <libusb.h>

class V800usb : public QObject
{
    Q_OBJECT
public:
    explicit V800usb(QObject *parent = 0);
    ~V800usb();

signals:
    void all_sessions(QList<QByteArray> sessions);
    void session_done();
    void ready();
    void not_ready();

public slots:
    void loop();
    void get_session(QByteArray session);

private:
    int extract_dir_and_files(unsigned char *full, int full_size, unsigned char dir_and_files[][64]);
    void generate_request(char *request, unsigned char *packet);
    void generate_ack(unsigned char packet_num, unsigned char *packet);
    int is_end(unsigned char *packet);
    int add_to_full(unsigned char *packet, unsigned char *full, int full_size, int initial_packet);
    int get_all_dates(libusb_device_handle *v800, unsigned char dates[][64]);
    int get_all_times(libusb_device_handle *v800, unsigned char *date, unsigned char times[][64]);
    int get_all_files(libusb_device_handle *v800, char *dir, unsigned char files[][64]);
    void get_file(libusb_device_handle *v800, char *dir, unsigned char *file);

    QList<QByteArray> get_all_dates();
    QList<QByteArray> get_all_times(QByteArray date);
    QList<QByteArray> get_all_files(QByteArray date, QByteArray time);
    void get_file(QByteArray date, QByteArray time, QByteArray file);
    void get_session_info(QByteArray date, QByteArray time);
    void get_all_sessions();

    libusb_device_handle *v800;
};

#endif // V800USB_H
