#include "v800main.h"
#include "ui_v800main.h"
#include "v800usb.h"

#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

V800Main::V800Main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::V800Main)
{
    qRegisterMetaType<QList<QString> >("QList<QString>");

    v800_ready = false;
    session_cnt = 0;

    QThread *usb_thread = new QThread;
    usb = new V800usb();
    usb->moveToThread(usb_thread);

    connect(usb, SIGNAL(all_sessions(QList<QString>)), this, SLOT(handle_all_sessions(QList<QString>)));
    connect(usb, SIGNAL(session_done()), this, SLOT(handle_session_done()));
    connect(usb, SIGNAL(ready()), this, SLOT(handle_ready()));
    connect(usb, SIGNAL(not_ready()), this, SLOT(handle_not_ready()));
    connect(this, SIGNAL(get_session(QString, QString, bool)), usb, SLOT(get_session(QString, QString, bool)));

    connect(usb_thread, SIGNAL(started()), usb, SLOT(start()));
    usb_thread->start();

    ui->setupUi(this);
    ui->exerciseTree->setColumnCount(1);
    ui->exerciseTree->setHeaderLabel("Session");

    ui->exerciseTree->setEnabled(false);
    ui->downloadBtn->setEnabled(false);
    ui->bipolarChk->setEnabled(false);
}

V800Main::~V800Main()
{
    delete ui;
    delete usb;
}

void V800Main::handle_not_ready()
{
    QMessageBox failure;
    failure.setText("Failed to open V800!");
    failure.setIcon(QMessageBox::Critical);
    failure.exec();

    exit(-1);
}

void V800Main::handle_ready()
{
    v800_ready = true;
    ui->exerciseTree->setEnabled(true);
    ui->downloadBtn->setEnabled(true);
    ui->bipolarChk->setEnabled(true);
}

void V800Main::handle_all_sessions(QList<QString> sessions)
{
    QList<QTreeWidgetItem *> items;
    int sessions_iter = 0;
    for(sessions_iter = 0; sessions_iter < sessions.length(); sessions_iter++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget *)0, QStringList(QString(sessions[sessions_iter])));
        item->setCheckState(0, Qt::Unchecked);
        items.append(item);
    }
    ui->exerciseTree->insertTopLevelItems(0, items);
}

void V800Main::handle_session_done()
{
    session_cnt--;
    if(session_cnt == 0)
    {
        ui->exerciseTree->setEnabled(true);
        ui->downloadBtn->setEnabled(true);
        ui->bipolarChk->setEnabled(true);
    }
}

void V800Main::on_downloadBtn_clicked()
{
    int item_iter;

    ui->exerciseTree->setEnabled(false);
    ui->downloadBtn->setEnabled(false);
    ui->bipolarChk->setEnabled(false);

    QString save_dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);
    if(save_dir == "")
    {
        ui->exerciseTree->setEnabled(true);
        ui->downloadBtn->setEnabled(true);
        ui->bipolarChk->setEnabled(true);

        return;
    }

    for(item_iter = 0; item_iter < ui->exerciseTree->topLevelItemCount(); item_iter++)
    {
        // get all the files if this exercise is checked
        if(ui->exerciseTree->topLevelItem(item_iter)->checkState(0) == Qt::Checked)
        {
            session_cnt++;
            emit get_session(ui->exerciseTree->topLevelItem(item_iter)->text(0), save_dir, ui->bipolarChk->isChecked());
        }
    }
}
