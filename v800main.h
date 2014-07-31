#ifndef V800MAIN_H
#define V800MAIN_H

#include <QWidget>

namespace Ui {
class V800Main;
}

class V800usb;

class V800Main : public QWidget
{
    Q_OBJECT

public:
    explicit V800Main(QWidget *parent = 0);
    ~V800Main();

signals:
    void get_session(QByteArray session, QString save_dir);

private slots:
    void handle_ready();
    void handle_not_ready();
    void handle_all_sessions(QList<QByteArray> sessions);
    void handle_session_done();
    void on_downloadBtn_clicked();

private:
    Ui::V800Main *ui;
    V800usb *usb;

    bool v800_ready;
    int session_cnt;
};

#endif // V800MAIN_H
