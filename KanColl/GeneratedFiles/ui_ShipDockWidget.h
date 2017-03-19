/********************************************************************************
** Form generated from reading UI file 'ShipDockWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHIPDOCKWIDGET_H
#define UI_SHIPDOCKWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShipDockWidget
{
public:
    QVBoxLayout *verticalLayout;
    QComboBox *cmbShipType;
    QComboBox *cmbModType;
    QListWidget *fileList;

    void setupUi(QWidget *ShipDockWidget)
    {
        if (ShipDockWidget->objectName().isEmpty())
            ShipDockWidget->setObjectName(QStringLiteral("ShipDockWidget"));
        ShipDockWidget->resize(400, 300);
        verticalLayout = new QVBoxLayout(ShipDockWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 3, 3, 1);
        cmbShipType = new QComboBox(ShipDockWidget);
        cmbShipType->setObjectName(QStringLiteral("cmbShipType"));
        cmbShipType->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

        verticalLayout->addWidget(cmbShipType);

        cmbModType = new QComboBox(ShipDockWidget);
        cmbModType->setObjectName(QStringLiteral("cmbModType"));

        verticalLayout->addWidget(cmbModType);

        fileList = new QListWidget(ShipDockWidget);
        fileList->setObjectName(QStringLiteral("fileList"));
        fileList->setContextMenuPolicy(Qt::ActionsContextMenu);
        fileList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        fileList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        fileList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        fileList->setAutoScrollMargin(16);
        fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        fileList->setProperty("showDropIndicator", QVariant(false));
        fileList->setTextElideMode(Qt::ElideNone);
        fileList->setResizeMode(QListView::Adjust);

        verticalLayout->addWidget(fileList);


        retranslateUi(ShipDockWidget);

        QMetaObject::connectSlotsByName(ShipDockWidget);
    } // setupUi

    void retranslateUi(QWidget *ShipDockWidget)
    {
        ShipDockWidget->setWindowTitle(QApplication::translate("ShipDockWidget", "ShipDockWidget", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ShipDockWidget: public Ui_ShipDockWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHIPDOCKWIDGET_H
