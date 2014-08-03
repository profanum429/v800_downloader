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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_V800Main
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *exerciseTree;
    QVBoxLayout *verticalLayout;
    QCheckBox *bipolarChk;
    QPushButton *downloadBtn;

    void setupUi(QWidget *V800Main)
    {
        if (V800Main->objectName().isEmpty())
            V800Main->setObjectName(QStringLiteral("V800Main"));
        V800Main->resize(452, 386);
        gridLayout = new QGridLayout(V800Main);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        exerciseTree = new QTreeWidget(V800Main);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        exerciseTree->setHeaderItem(__qtreewidgetitem);
        exerciseTree->setObjectName(QStringLiteral("exerciseTree"));

        horizontalLayout->addWidget(exerciseTree);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        bipolarChk = new QCheckBox(V800Main);
        bipolarChk->setObjectName(QStringLiteral("bipolarChk"));

        verticalLayout->addWidget(bipolarChk);

        downloadBtn = new QPushButton(V800Main);
        downloadBtn->setObjectName(QStringLiteral("downloadBtn"));

        verticalLayout->addWidget(downloadBtn);


        horizontalLayout->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);


        retranslateUi(V800Main);

        QMetaObject::connectSlotsByName(V800Main);
    } // setupUi

    void retranslateUi(QWidget *V800Main)
    {
        V800Main->setWindowTitle(QApplication::translate("V800Main", "V800Main", 0));
        bipolarChk->setText(QApplication::translate("V800Main", "Include\n"
"Bipolar \n"
"Output", 0));
        downloadBtn->setText(QApplication::translate("V800Main", "Download\n"
"Exercises", 0));
    } // retranslateUi

};

namespace Ui {
    class V800Main: public Ui_V800Main {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_V800MAIN_H
