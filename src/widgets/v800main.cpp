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

#include "v800main.h"
#include "ui_v800main.h"
#include "v800usb.h"
#include "v800export.h"
#include "v800fs.h"

#include <QtWidgets>

V800Main::V800Main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::V800Main)
{
    qRegisterMetaType<QList<QString> >("QList<QString>");

    v800_ready = false;

    QThread *usb_thread = new QThread;
    usb = new V800usb();
    usb->moveToThread(usb_thread);

    QThread *export_data_thread = new QThread;
    export_data = new V800export();
    export_data->moveToThread(export_data_thread);

    connect(usb, SIGNAL(all_sessions(QList<QString>)), this, SLOT(handle_all_sessions(QList<QString>)));
    connect(usb, SIGNAL(sessions_done()), this, SLOT(handle_sessions_done()));
    connect(usb, SIGNAL(session_done(QString, int, int)), this, SLOT(handle_session_done(QString, int, int)));
    connect(usb, SIGNAL(ready()), this, SLOT(handle_ready()));
    connect(usb, SIGNAL(not_ready()), this, SLOT(handle_not_ready()));

    connect(export_data, SIGNAL(export_session_done(int,int)), this, SLOT(handle_export_session_done(int,int)));
    connect(export_data, SIGNAL(export_sessions_done()), this, SLOT(handle_export_sessions_done()));
    connect(export_data, SIGNAL(export_session_error(QString,int)), this, SLOT(handle_export_session_error(QString,int)));

    connect(this, SIGNAL(get_sessions(QList<QString>)), usb, SLOT(get_sessions(QList<QString>)));
    connect(this, SIGNAL(export_sessions(QList<QString>,unsigned char)), export_data, SLOT(export_sessions(QList<QString>,unsigned char)));

    new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_A), this, SLOT(handle_advanced_shortcut()));

    connect(usb_thread, SIGNAL(started()), usb, SLOT(start()));
    usb_thread->start();

    connect(export_data_thread, SIGNAL(started()), export_data, SLOT(start()));
    export_data_thread->start();

    QCoreApplication::setOrganizationName(tr("profanum429"));
    QCoreApplication::setOrganizationDomain(tr("profanum429.com"));
    QCoreApplication::setApplicationName(tr("V800 Downloader"));

    QSettings settings;
    QString default_dir = settings.value(tr("default_dir")).toString();
    if(default_dir.isEmpty())
    {
        default_dir = QString((QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QString(tr("/Polar/PolarFlowSync/export"))));
        settings.setValue(tr("default_dir"), default_dir);
    }

    QString file_dir = settings.value(tr("file_dir")).toString();
    if(file_dir.isEmpty())
    {
        file_dir = QString((QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QString(tr("/Polar/PolarFlowSync/export"))));
        settings.setValue(tr("file_dir"), file_dir);
    }


    ui->setupUi(this);
    ui->verticalLayout->setAlignment(Qt::AlignTop);
    ui->verticalLayout->setSpacing(20);
    ui->exerciseTree->setColumnCount(1);
    ui->exerciseTree->setHeaderLabel(tr("Session"));

    ui->fsBtn->setVisible(false);

    ui->tcxBox->setChecked(true);

    disable_all();
    this->show();

    start_in_progress = new QMessageBox(this);
    start_in_progress->setWindowTitle(tr("V800 Downloader"));
    start_in_progress->setText(tr("Downloading session list from V800..."));
    start_in_progress->setIcon(QMessageBox::Information);
    start_in_progress->setStandardButtons(0);
    start_in_progress->setWindowModality(Qt::WindowModal);
    start_in_progress->exec();
}

V800Main::~V800Main()
{
    delete ui;
    delete usb;
}

void V800Main::handle_not_ready()
{
    QMessageBox failure;
    failure.setWindowTitle(tr("V800 Downloader"));
    failure.setText(tr("Failed to open V800!"));
    failure.setIcon(QMessageBox::Critical);
    failure.exec();

    exit(-1);
}

void V800Main::handle_ready()
{
    start_in_progress->done(0);

    v800_ready = true;
    enable_all();
}

void V800Main::handle_all_sessions(QList<QString> sessions)
{
    QList<QTreeWidgetItem *> items;

    for(int sessions_iter = 0; sessions_iter < sessions.length(); sessions_iter++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget *)0, QStringList(sessions[sessions_iter]));
        item->setCheckState(0, Qt::Unchecked);
        items.append(item);
    }
    ui->exerciseTree->insertTopLevelItems(0, items);
}

void V800Main::handle_session_done(QString session, int session_iter, int session_cnt)
{
    sessions_to_export.append(session);

    download_progress->setValue(session_iter+1);
    download_progress->setLabelText(tr("Downloading %1/%2...").arg(session_iter+2).arg(session_cnt));
}

void V800Main::handle_sessions_done()
{
    download_progress->setValue(1);
    download_progress->setLabelText(tr("Exporting 1/%2...").arg(sessions_to_export.length()));
    download_progress->show();

    unsigned char export_mask = (ui->tcxBox->isChecked() ? V800export::TCX_EXPORT : 0x00) |
                                (ui->hrmBox->isChecked() ? V800export::HRM_EXPORT : 0x00) |
                                (ui->gpxBox->isChecked() ? V800export::GPX_EXPORT : 0x00);
    emit export_sessions(sessions_to_export, export_mask);
}

void V800Main::handle_export_session_done(int session_iter, int session_cnt)
{
    download_progress->setValue(session_iter+1);
    download_progress->setLabelText(tr("Exporting %1/%2...").arg(session_iter+2).arg(session_cnt));
}

void V800Main::handle_export_sessions_done()
{
    download_progress->done(0);

    QSettings settings;
    QString default_dir = settings.value(tr("default_dir")).toString();
    QString msg_text(QString(tr("Done downloading and converting sessions\n\nLook in \n%1\nto find your files")).arg(default_dir));

    if(error_list.length() > 0)
        msg_text.append(tr("\n\nErrors occurred during processing\n\n"));
    for(int error_iter = 0; error_iter < error_list.length(); error_iter++)
        msg_text.append(QString(tr("%1\n")).arg(error_list[error_iter]));

    QMessageBox done;
    done.setWindowTitle(tr("V800 Downloader"));
    done.setText(msg_text);
    done.setIcon(QMessageBox::Information);
    done.setWindowModality(Qt::WindowModal);
    done.exec();

    enable_all();
}

void V800Main::handle_export_session_error(QString session, int error)
{
    if(error == V800export::RENAME_ERROR)
        error_list.append(QString(tr("%1: Error making Bipolar names")).arg(session));

    if(error == V800export::PARSE_ERROR)
        error_list.append(QString(tr("%1: Error parsing session data")).arg(session));

    if(error == V800export::TCX_ERROR)
        error_list.append(QString(tr("%1: Error making TCX file")).arg(session));

    if(error == V800export::HRM_ERROR)
        error_list.append(QString(tr("%1: Error making HRM file")).arg(session));

    if(error == V800export::GPX_ERROR)
        error_list.append(QString(tr("%1: Error making GPX file")).arg(session));
}

void V800Main::handle_advanced_shortcut()
{
    if(ui->fsBtn->isVisible())
        ui->fsBtn->setVisible(false);
    else
        ui->fsBtn->setVisible(true);
}

void V800Main::enable_all()
{
    ui->downloadBtn->setEnabled(true);
    ui->checkBtn->setEnabled(true);
    ui->uncheckBtn->setEnabled(true);
    ui->fsBtn->setEnabled(true);
    ui->dirSelectBtn->setEnabled(true);
    ui->tcxBox->setEnabled(true);
    ui->hrmBox->setEnabled(true);
    ui->gpxBox->setEnabled(true);
}

void V800Main::disable_all()
{
    ui->downloadBtn->setEnabled(false);
    ui->checkBtn->setEnabled(false);
    ui->uncheckBtn->setEnabled(false);
    ui->fsBtn->setEnabled(false);
    ui->dirSelectBtn->setEnabled(false);
    ui->tcxBox->setEnabled(false);
    ui->hrmBox->setEnabled(false);
    ui->gpxBox->setEnabled(false);
}

void V800Main::on_downloadBtn_clicked()
{
    QList<QString> sessions;
    unsigned char export_mask = 0x00, export_exists = 0x00;
    int item_iter;

    sessions_to_export.clear();
    disable_all();

    QSettings settings;
    QString default_dir = settings.value(tr("default_dir")).toString();

    for(item_iter = 0; item_iter < ui->exerciseTree->topLevelItemCount(); item_iter++)
    {
        export_mask = 0x00;
        export_exists = 0x00;

        if(ui->exerciseTree->topLevelItem(item_iter)->checkState(0) == Qt::Checked)
        {
            export_mask = (ui->tcxBox->isChecked() ? V800export::TCX_EXPORT : 0x00) |
                          (ui->hrmBox->isChecked() ? V800export::HRM_EXPORT : 0x00) |
                          (ui->gpxBox->isChecked() ? V800export::GPX_EXPORT : 0x00);

            QStringList session_split = (QDateTime::fromString(ui->exerciseTree->topLevelItem(item_iter)->text(0), Qt::TextDate)).toString(tr("yyyyMMdd/HHmmss")).split(tr("/"));
            QString tag = QDateTime(QDate::fromString(session_split[0], tr("yyyyMMdd")), QTime::fromString(session_split[1], tr("HHmmss"))).toString(tr("yyyyMMddhhmmss"));

            if(export_mask & V800export::TCX_EXPORT)
                if(QFile::exists(QString(tr("%1/%2.tcx")).arg(default_dir).arg(tag)))
                    export_exists |= V800export::TCX_EXPORT;
            if(export_mask & V800export::HRM_EXPORT)
                if(QFile::exists(QString(tr("%1/%2.hrm")).arg(default_dir).arg(tag)))
                    export_exists |= V800export::HRM_EXPORT;
            if(export_mask & V800export::GPX_EXPORT)
                if(QFile::exists(QString(tr("%1/%2.gpx")).arg(default_dir).arg(tag)))
                    export_exists |= V800export::GPX_EXPORT;

            if(export_mask != export_exists)
                sessions.append(ui->exerciseTree->topLevelItem(item_iter)->text(0));
        }
    }

    if(sessions.length() > 0)
    {
        download_progress = new QProgressDialog(tr("Downloading 1/%1...").arg(sessions.length()), tr("Cancel"), 0, sessions.length(), this);
        download_progress->setCancelButton(0);
        download_progress->setWindowModality(Qt::WindowModal);
        download_progress->setValue(0);
        download_progress->setWindowTitle(tr("V800 Downloader"));
        download_progress->show();

        emit get_sessions(sessions);
    }
    else
    {
        QString msg_text(QString(tr("All selected sessions have already been downloaded and exported\n\nLook in \n%1\nto find your files")).arg(default_dir));

        QMessageBox done;
        done.setWindowTitle(tr("V800 Downloader"));
        done.setText(msg_text);
        done.setIcon(QMessageBox::Information);
        done.setWindowModality(Qt::WindowModal);
        done.exec();

        enable_all();
    }
}

void V800Main::on_checkBtn_clicked()
{
    int item_iter;

    for(item_iter = 0; item_iter < ui->exerciseTree->topLevelItemCount(); item_iter++)
        ui->exerciseTree->topLevelItem(item_iter)->setCheckState(0, Qt::Checked);
}

void V800Main::on_uncheckBtn_clicked()
{
    int item_iter;

    for(item_iter = 0; item_iter < ui->exerciseTree->topLevelItemCount(); item_iter++)
        ui->exerciseTree->topLevelItem(item_iter)->setCheckState(0, Qt::Unchecked);
}

void V800Main::on_fsBtn_clicked()
{
    V800fs *fs = new V800fs(usb);
    fs->setWindowModality(Qt::WindowModal);
    fs->show();
}

void V800Main::on_dirSelectBtn_clicked()
{
    QSettings settings;
    QString default_dir = settings.value(tr("default_dir")).toString();

    QString new_default_dir = QFileDialog::getExistingDirectory(this, tr("Default Save Directory"), default_dir, QFileDialog::ShowDirsOnly);
    if(!new_default_dir.isEmpty())
    {
        QSettings settings;
        settings.setValue(tr("default_dir"), new_default_dir);
    }
}
