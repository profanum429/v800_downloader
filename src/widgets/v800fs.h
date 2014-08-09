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
