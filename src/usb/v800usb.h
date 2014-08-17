#ifndef V800USB_H
#define V800USB_H

#include <QObject>

#define V800_ROOT_DIR   "/U/0"

class native_usb;

class V800usb : public QObject
{
    Q_OBJECT
public:
    explicit V800usb(QObject *parent = 0);
    ~V800usb();

    enum {
        PARSE_FAILED = 1,
        TCX_FAILED,
        HRM_FAILED,
        GPX_FAILED,
        TCX_EXISTS,
        HRM_EXISTS,
        GPX_EXISTS,
        FAILURES
    };

    enum {
        TCX = 0,
        HRM,
        GPX,
        EXPORT_TYPES
    };

signals:
    void all_sessions(QList<QString> sessions);
    void session_done();
    void session_failed(QString tag, int failure);
    void sessions_done();
    void ready();
    void not_ready();

    void all_objects(QList<QString> objects);
    void file_done();

public slots:
    void start();
    void get_sessions(QList<QString> sessions, QString default_dir, bool raw_output, bool tcx_output, bool hrm_output, bool gpx_output);

    void get_all_objects(QString path);
    void get_file(QString path, QString file_dir);

private:
    QList<QString> extract_dir_and_files(QByteArray full);
    QByteArray generate_request(QString request);
    QByteArray generate_ack(unsigned char packet_num);
    int is_end(QByteArray packet);
    QByteArray add_to_full(QByteArray packet, QByteArray full, bool initial_packet, bool final_packet);

    QList<QString> get_v800_data(QString request, bool debug=false);

    void get_all_sessions();

    native_usb *usb;

    QString default_dir;
    QString raw_dir;
    QString file_dir;

    bool raw_output;
};

#endif // V800USB_H
