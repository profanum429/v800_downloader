/********************************************************************************
** Form generated from reading UI file 'v800main.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_V800MAIN_H
#define UI_V800MAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_V800Main
{
public:
    QPushButton *downloadBtn;
    QTreeWidget *exerciseTree;

    void setupUi(QWidget *V800Main)
    {
        if (V800Main->objectName().isEmpty())
            V800Main->setObjectName(QStringLiteral("V800Main"));
        V800Main->resize(400, 300);
        downloadBtn = new QPushButton(V800Main);
        downloadBtn->setObjectName(QStringLiteral("downloadBtn"));
        downloadBtn->setGeometry(QRect(294, 220, 81, 41));
        exerciseTree = new QTreeWidget(V800Main);
        exerciseTree->setObjectName(QStringLiteral("exerciseTree"));
        exerciseTree->setGeometry(QRect(10, 10, 271, 281));

        retranslateUi(V800Main);

        QMetaObject::connectSlotsByName(V800Main);
    } // setupUi

    void retranslateUi(QWidget *V800Main)
    {
        V800Main->setWindowTitle(QApplication::translate("V800Main", "V800Main", 0));
        downloadBtn->setText(QApplication::translate("V800Main", "Download\n"
"Exercises", 0));
    } // retranslateUi

};

namespace Ui {
    class V800Main: public Ui_V800Main {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_V800MAIN_H
