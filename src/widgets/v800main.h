#ifndef V800MAIN_H
#define V800MAIN_H

#include <QWidget>

namespace Ui {
class V800Main;
}

class V800usb;
class QMessageBox;
class QProgressDialog;

class V800Main : public QWidget
{
    Q_OBJECT

public:
    explicit V800Main(QWidget *parent = 0);
    ~V800Main();

signals:
    void get_sessions(QList<QString> session, QString save_dir, bool bipolar_output);

private slots:
    void handle_ready();
    void handle_not_ready();
    void handle_all_sessions(QList<QString> sessions);
    void handle_session_done();
    void handle_sessions_done();
    void handle_advanced_shortcut();

    void on_downloadBtn_clicked();
    void on_checkBtn_clicked();
    void on_uncheckBtn_clicked();
    void on_fsBtn_clicked();

    void on_dirSelectBtn_clicked();

private:
    void disable_all();
    void enable_all();

    Ui::V800Main *ui;
    V800usb *usb;

    QMessageBox *start_in_progress;
    QProgressDialog *download_progress;

    bool v800_ready;
    int sessions_cnt;

    QString default_dir;
};

#endif // V800MAIN_H
