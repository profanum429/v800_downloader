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

#ifndef V800MAIN_H
#define V800MAIN_H

#include <QWidget>

namespace Ui {
class V800Main;
}

class V800usb;
class V800export;
class QMessageBox;
class QProgressDialog;

class V800Main : public QWidget
{
    Q_OBJECT

public:
    explicit V800Main(QWidget *parent = 0);
    ~V800Main();

signals:
    void get_sessions(QList<QString> session);
    void export_sessions(QList<QString> sessions, unsigned char mode);

private slots:
    void handle_ready();
    void handle_not_ready();
    void handle_all_sessions(QList<QString> sessions);
    void handle_session_done(QString session, int session_iter, int session_cnt);
    void handle_sessions_done();

    void handle_export_session_done(int session_iter, int session_cnt);
    void handle_export_sessions_done();
    void handle_export_session_error(QString session, int error);

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
    V800export *export_data;

    QMessageBox *start_in_progress;
    QProgressDialog *download_progress;

    bool v800_ready;
    QList<QString> sessions_to_export;
    QList<QString> error_list;
};

#endif // V800MAIN_H
