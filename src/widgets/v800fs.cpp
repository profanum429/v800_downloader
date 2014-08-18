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

#include "v800fs.h"
#include "ui_v800fs.h"
#include "v800usb.h"

#include <QtWidgets>

V800fs::V800fs(V800usb *usb, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::V800fs),
    usb(usb)
{
    qRegisterMetaType<QList<QString> >("QList<QString>");

    connect(usb, SIGNAL(all_objects(QList<QString>)), this, SLOT(handle_all_objects(QList<QString>)));
    connect(usb, SIGNAL(file_done()), this, SLOT(handle_file_done()));

    connect(this, SIGNAL(get_objects(QString)), usb, SLOT(get_all_objects(QString)));
    connect(this, SIGNAL(get_file(QString, QString)), usb, SLOT(get_file(QString, QString)));

    ui->setupUi(this);
    ui->v800Tree->setColumnCount(1);
    ui->v800Tree->setHeaderLabel(tr("V800 FS"));

    QTreeWidgetItem *top = new QTreeWidgetItem((QTreeWidget *)0, QStringList(tr(V800_FS_ROOT)));
    QTreeWidgetItem *temp_child = new QTreeWidgetItem((QTreeWidget *)0, QStringList(tr("000TEMP000")));
    top->addChild(temp_child);
    ui->v800Tree->insertTopLevelItem(0, top);
}

V800fs::~V800fs()
{
    delete ui;
}

void V800fs::handle_all_objects(QList<QString> objects)
{
    for(int objects_iter = 0; objects_iter < objects.length(); objects_iter++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget *)0, QStringList(objects[objects_iter]));
        if(objects[objects_iter].contains(tr("/")))
        {
            QTreeWidgetItem *temp_child = new QTreeWidgetItem((QTreeWidget *)0, QStringList(tr("000TEMP000")));
            item->addChild(temp_child);
        }
        expanded->addChild(item);
    }
}

void V800fs::handle_file_done()
{
    download_progress->done(0);

    enable_all();
}

void V800fs::enable_all()
{
    ui->v800Tree->setEnabled(true);
    if(ui->v800Tree->selectedItems()[0]->text(0).contains(tr("/")))
        ui->downloadBtn->setEnabled(false);
    else
        ui->downloadBtn->setEnabled(true);
}

void V800fs::disable_all()
{
    ui->v800Tree->setEnabled(false);
    ui->downloadBtn->setEnabled(false);
}

void V800fs::on_v800Tree_itemExpanded(QTreeWidgetItem *item)
{
    if(item->childCount() == 1)
    {
        if(item->child(0)->text(0).compare(tr("000TEMP000")) == 0)
            item->removeChild(item->child(0));
    }

    QTreeWidgetItem *parent=NULL;
    QString path;

    parent = item->parent();
    path.prepend(item->text(0));
    while(parent != NULL)
    {
        path.prepend(parent->text(0));
        parent = parent->parent();
    }

    qDebug("Path: %s", path.toUtf8().constData());

    expanded = item;
    emit get_objects(path);
}

void V800fs::on_downloadBtn_clicked()
{
    QString save_dir;

    disable_all();

    save_dir = QFileDialog::getExistingDirectory(this, tr("Save Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);
    if(save_dir == tr(""))
    {
        enable_all();
        return;
    }

    download_progress = new QProgressDialog(tr("Downloading %1...").arg(ui->v800Tree->selectedItems()[0]->text(0)), tr("Cancel"), 0, 2, this);
    download_progress->setCancelButton(0);
    download_progress->setWindowModality(Qt::WindowModal);
    download_progress->setValue(1);
    download_progress->setWindowTitle(tr("V800 Downloader"));
    download_progress->show();

    QTreeWidgetItem *parent=NULL;
    QString path;

    parent = ui->v800Tree->selectedItems()[0]->parent();
    path.prepend(ui->v800Tree->selectedItems()[0]->text(0));
    while(parent != NULL)
    {
        path.prepend(parent->text(0));
        parent = parent->parent();
    }

    qDebug("Path: %s", path.toUtf8().constData());
    emit get_file(path, save_dir);
}

void V800fs::on_v800Tree_itemSelectionChanged()
{
    if(ui->v800Tree->selectedItems()[0]->text(0).contains(tr("/")))
        ui->downloadBtn->setEnabled(false);
    else
        ui->downloadBtn->setEnabled(true);
}
