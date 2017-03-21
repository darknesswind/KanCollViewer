/********************************************************************************
** Form generated from reading UI file 'KanColl.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KANCOLL_H
#define UI_KANCOLL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "PreviewWidget.h"
#include "ShipDockWidget.h"

QT_BEGIN_NAMESPACE

class Ui_QtGuiApplication1Class
{
public:
    QAction *actionChange_Wall;
    QAction *actionChange_Floor;
    QAction *actionChange_Desk;
    QAction *actionChange_Window;
    QAction *actionChange_Object;
    QAction *actionChange_Chest;
    QAction *actionExportSwf;
    QAction *actionSavePreview;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *rbHome;
    QRadioButton *rbAlbum;
    PreviewWidget *preview;
    QMenuBar *menuBar;
    QMenu *menu_Room;
    QMenu *menuTools;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *shipDock;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    ShipDockWidget *shipDockWidget;
    QDockWidget *dockWidget_2;
    QWidget *dockWidgetContents_2;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabDetails;

    void setupUi(QMainWindow *QtGuiApplication1Class)
    {
        if (QtGuiApplication1Class->objectName().isEmpty())
            QtGuiApplication1Class->setObjectName(QStringLiteral("QtGuiApplication1Class"));
        QtGuiApplication1Class->resize(953, 602);
        actionChange_Wall = new QAction(QtGuiApplication1Class);
        actionChange_Wall->setObjectName(QStringLiteral("actionChange_Wall"));
        actionChange_Floor = new QAction(QtGuiApplication1Class);
        actionChange_Floor->setObjectName(QStringLiteral("actionChange_Floor"));
        actionChange_Desk = new QAction(QtGuiApplication1Class);
        actionChange_Desk->setObjectName(QStringLiteral("actionChange_Desk"));
        actionChange_Window = new QAction(QtGuiApplication1Class);
        actionChange_Window->setObjectName(QStringLiteral("actionChange_Window"));
        actionChange_Object = new QAction(QtGuiApplication1Class);
        actionChange_Object->setObjectName(QStringLiteral("actionChange_Object"));
        actionChange_Chest = new QAction(QtGuiApplication1Class);
        actionChange_Chest->setObjectName(QStringLiteral("actionChange_Chest"));
        actionExportSwf = new QAction(QtGuiApplication1Class);
        actionExportSwf->setObjectName(QStringLiteral("actionExportSwf"));
        actionSavePreview = new QAction(QtGuiApplication1Class);
        actionSavePreview->setObjectName(QStringLiteral("actionSavePreview"));
        centralWidget = new QWidget(QtGuiApplication1Class);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        rbHome = new QRadioButton(centralWidget);
        rbHome->setObjectName(QStringLiteral("rbHome"));
        rbHome->setChecked(true);

        horizontalLayout_2->addWidget(rbHome);

        rbAlbum = new QRadioButton(centralWidget);
        rbAlbum->setObjectName(QStringLiteral("rbAlbum"));

        horizontalLayout_2->addWidget(rbAlbum);


        verticalLayout_2->addLayout(horizontalLayout_2);

        preview = new PreviewWidget(centralWidget);
        preview->setObjectName(QStringLiteral("preview"));

        verticalLayout_2->addWidget(preview);

        QtGuiApplication1Class->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtGuiApplication1Class);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 953, 23));
        menu_Room = new QMenu(menuBar);
        menu_Room->setObjectName(QStringLiteral("menu_Room"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        QtGuiApplication1Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtGuiApplication1Class);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtGuiApplication1Class->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtGuiApplication1Class);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtGuiApplication1Class->setStatusBar(statusBar);
        shipDock = new QDockWidget(QtGuiApplication1Class);
        shipDock->setObjectName(QStringLiteral("shipDock"));
        shipDock->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        shipDockWidget = new ShipDockWidget(dockWidgetContents);
        shipDockWidget->setObjectName(QStringLiteral("shipDockWidget"));

        verticalLayout->addWidget(shipDockWidget);

        shipDock->setWidget(dockWidgetContents);
        QtGuiApplication1Class->addDockWidget(static_cast<Qt::DockWidgetArea>(1), shipDock);
        dockWidget_2 = new QDockWidget(QtGuiApplication1Class);
        dockWidget_2->setObjectName(QStringLiteral("dockWidget_2"));
        dockWidget_2->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
        horizontalLayout = new QHBoxLayout(dockWidgetContents_2);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabDetails = new QTabWidget(dockWidgetContents_2);
        tabDetails->setObjectName(QStringLiteral("tabDetails"));

        horizontalLayout->addWidget(tabDetails);

        dockWidget_2->setWidget(dockWidgetContents_2);
        QtGuiApplication1Class->addDockWidget(static_cast<Qt::DockWidgetArea>(8), dockWidget_2);

        menuBar->addAction(menu_Room->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menu_Room->addAction(actionChange_Wall);
        menu_Room->addAction(actionChange_Floor);
        menu_Room->addAction(actionChange_Desk);
        menu_Room->addAction(actionChange_Window);
        menu_Room->addAction(actionChange_Object);
        menu_Room->addAction(actionChange_Chest);
        menuTools->addAction(actionSavePreview);
        menuTools->addAction(actionExportSwf);

        retranslateUi(QtGuiApplication1Class);

        QMetaObject::connectSlotsByName(QtGuiApplication1Class);
    } // setupUi

    void retranslateUi(QMainWindow *QtGuiApplication1Class)
    {
        QtGuiApplication1Class->setWindowTitle(QApplication::translate("QtGuiApplication1Class", "QtGuiApplication1", Q_NULLPTR));
        actionChange_Wall->setText(QApplication::translate("QtGuiApplication1Class", "Change W&all", Q_NULLPTR));
        actionChange_Floor->setText(QApplication::translate("QtGuiApplication1Class", "Change &Floor", Q_NULLPTR));
        actionChange_Desk->setText(QApplication::translate("QtGuiApplication1Class", "Change &Desk", Q_NULLPTR));
        actionChange_Window->setText(QApplication::translate("QtGuiApplication1Class", "Change &Window", Q_NULLPTR));
        actionChange_Object->setText(QApplication::translate("QtGuiApplication1Class", "Change &Object", Q_NULLPTR));
        actionChange_Chest->setText(QApplication::translate("QtGuiApplication1Class", "Change &Chest", Q_NULLPTR));
        actionExportSwf->setText(QApplication::translate("QtGuiApplication1Class", "&Export Image From Swf ", Q_NULLPTR));
        actionSavePreview->setText(QApplication::translate("QtGuiApplication1Class", "&Save Preview", Q_NULLPTR));
        rbHome->setText(QApplication::translate("QtGuiApplication1Class", "Homeport", Q_NULLPTR));
        rbAlbum->setText(QApplication::translate("QtGuiApplication1Class", "Album", Q_NULLPTR));
        menu_Room->setTitle(QApplication::translate("QtGuiApplication1Class", "Modify &Room", Q_NULLPTR));
        menuTools->setTitle(QApplication::translate("QtGuiApplication1Class", "&Tools", Q_NULLPTR));
        shipDock->setWindowTitle(QApplication::translate("QtGuiApplication1Class", "Ships", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtGuiApplication1Class: public Ui_QtGuiApplication1Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KANCOLL_H
