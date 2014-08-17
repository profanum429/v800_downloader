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
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef V800FS_H
#define V800FS_H

#include <QWidget>

#define V800_FS_ROOT   "/"

namespace Ui {
class V800fs;
}

class V800usb;
class QTreeWidgetItem;
class QProgressDialog;

class V800fs : public QWidget
{
    Q_OBJECT

public:
    explicit V800fs(V800usb *usb, QWidget *parent = 0);
    ~V800fs();

signals:
    void get_objects(QString path);
    void get_file(QString path, QString save_dir);

public slots:
    void handle_all_objects(QList<QString> objects);
    void handle_file_done();

private slots:
    void on_v800Tree_itemExpanded(QTreeWidgetItem *item);

    void on_downloadBtn_clicked();

    void on_v800Tree_itemSelectionChanged();

private:
    void enable_all();
    void disable_all();

    Ui::V800fs *ui;

    V800usb *usb;
    QTreeWidgetItem *expanded;
    QProgressDialog *download_progress;
};

#endif // V800FS_H
