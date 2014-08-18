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

    connect(usb, SIGNAL(all_sessions(QList<QString>)), this, SLOT(handle_all_sessions(QList<QString>)));
    connect(usb, SIGNAL(sessions_done()), this, SLOT(handle_sessions_done()));
    connect(usb, SIGNAL(session_done()), this, SLOT(handle_session_done()));
    connect(usb, SIGNAL(ready()), this, SLOT(handle_ready()));
    connect(usb, SIGNAL(not_ready()), this, SLOT(handle_not_ready()));

    connect(this, SIGNAL(get_sessions(QList<QString>, QString)), usb, SLOT(get_sessions(QList<QString>, QString)));

    new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_A), this, SLOT(handle_advanced_shortcut()));

    connect(usb_thread, SIGNAL(started()), usb, SLOT(start()));
    usb_thread->start();

    QCoreApplication::setOrganizationName(tr("profanum429"));
    QCoreApplication::setOrganizationDomain(tr("profanum429.com"));
    QCoreApplication::setApplicationName(tr("V800 Downloader"));

    QSettings settings;
    default_dir = settings.value(tr("default_dir")).toString();
    if(default_dir.isEmpty())
    {
        qDebug("Default dir empty!");
        default_dir = QString((QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QString(tr("/Polar/PolarFlowSync/export"))));
        settings.setValue(tr("default_dir"), default_dir);
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

void V800Main::handle_session_done()
{
    int cur_session;

    cur_session = download_progress->value();
    download_progress->setValue(cur_session+1);
    download_progress->setLabelText(tr("Downloading %1/%2...").arg(cur_session+1).arg(sessions_cnt));
}

/*
void V800Main::handle_session_failed(QString tag, int failure)
{
    if(failure == V800usb::PARSE_FAILED)
        error_list.append(QString(tr("Error! %1: Invalid session files!")).arg(tag));
    else if(failure == V800usb::TCX_FAILED)
        error_list.append(QString(tr("Error! %1: Failed to write TCX file!")).arg(tag));
    else if(failure == V800usb::HRM_FAILED)
        error_list.append(QString(tr("Error! %1: Failed to write HRM file!")).arg(tag));
    else if(failure == V800usb::GPX_FAILED)
        error_list.append(QString(tr("Error! %1: Failed to write GPX file!")).arg(tag));

    else if(failure == V800usb::TCX_EXISTS)
        error_list.append(QString(tr("Warning: TCX for %1 already exists, skipping.")).arg(tag));
    else if(failure == V800usb::HRM_EXISTS)
        error_list.append(QString(tr("Warning: HRM for %1 already exists, skipping.")).arg(tag));
    else if(failure == V800usb::GPX_EXISTS)
        error_list.append(QString(tr("Warning: GPX for %1 already exists, skipping.")).arg(tag));

}
*/

void V800Main::handle_sessions_done()
{
    download_progress->done(0);

    QString msg_text(QString(tr("Done downloading and converting sessions!\nLook in %1 to find your files!")).arg(default_dir));

    if(error_list.length() > 0)
    {
        msg_text.append(QString(tr("\n\nThe following issues occurred during downloading and conversion\n")));
        for(int error_iter = 0; error_iter < error_list.length(); error_iter++)
            msg_text.append(QString(tr("\n%1")).arg(error_list[error_iter]));
    }

    QMessageBox done;
    done.setWindowTitle(tr("V800 Downloader"));
    done.setText(msg_text);
    done.setIcon(QMessageBox::Information);
    done.setWindowModality(Qt::WindowModal);
    done.exec();

    enable_all();
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
    int item_iter;

    disable_all();
    sessions_cnt = 0;

    for(item_iter = 0; item_iter < ui->exerciseTree->topLevelItemCount(); item_iter++)
    {
        if(ui->exerciseTree->topLevelItem(item_iter)->checkState(0) == Qt::Checked)
        {
            sessions_cnt++;
            sessions.append(ui->exerciseTree->topLevelItem(item_iter)->text(0));
        }
    }

    error_list.clear();

    download_progress = new QProgressDialog(tr("Downloading 1/%1...").arg(sessions_cnt), tr("Cancel"), 0, sessions_cnt+1, this);
    download_progress->setCancelButton(0);
    download_progress->setWindowModality(Qt::WindowModal);
    download_progress->setValue(1);
    download_progress->setWindowTitle(tr("V800 Downloader"));
    download_progress->show();

    emit get_sessions(sessions, default_dir);
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
    QString new_default_dir = QFileDialog::getExistingDirectory(this, tr("Default Save Directory"), default_dir, QFileDialog::ShowDirsOnly);
    if(!new_default_dir.isEmpty())
    {
        default_dir = new_default_dir;

        QSettings settings;
        settings.setValue(tr("default_dir"), default_dir);
    }
}
