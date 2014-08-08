#ifndef V800USB_H
#define V800USB_H

#include <QObject>

class native_usb;

class V800usb : public QObject
{
    Q_OBJECT
public:
    explicit V800usb(QObject *parent = 0);
    ~V800usb();

signals:
    void all_sessions(QList<QString> sessions);
    void session_done();
    void sessions_done();
    void ready();
    void not_ready();

public slots:
    void start();
    void get_sessions(QList<QString> sessions, QString save_dir, bool raw_output);

private:
    QList<QString> extract_dir_and_files(QByteArray full);
    QByteArray generate_request(QString request);
    QByteArray generate_ack(unsigned char packet_num);
    int is_end(QByteArray packet);
    QByteArray add_to_full(QByteArray packet, QByteArray full, bool initial_packet, bool final_packet);

    QList<QString> get_all_dates();
    QList<QString> get_all_times(QString date);
    QList<QString> get_all_files(QString date, QString time);
    void get_file(QString date, QString time, QString file, int type);
    void get_session_info(QString date, QString time);

    void get_all_sessions();

    native_usb *usb;

    QString save_dir;
    bool raw_output;

    enum {
        SESSION_DATA = 0,
        SESSION_INFO = 1,
        END_TYPES
    };
};

#endif // V800USB_H
