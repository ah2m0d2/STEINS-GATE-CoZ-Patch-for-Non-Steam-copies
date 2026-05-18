/********************************************************************************
** Form generated from reading UI file 'installerwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INSTALLERWINDOW_H
#define UI_INSTALLERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InstallerWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *headerImage;
    QHBoxLayout *belowHeaderColumns;
    QLabel *leftImage;
    QVBoxLayout *verticalLayout_2;
    QStackedWidget *stackedWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *backButton;
    QPushButton *nextButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *cancelButton;
    QLabel *rightImage;

    void setupUi(QMainWindow *InstallerWindow)
    {
        if (InstallerWindow->objectName().isEmpty())
            InstallerWindow->setObjectName(QString::fromUtf8("InstallerWindow"));
        InstallerWindow->resize(500, 580);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(InstallerWindow->sizePolicy().hasHeightForWidth());
        InstallerWindow->setSizePolicy(sizePolicy);
        InstallerWindow->setMinimumSize(QSize(500, 580));
        InstallerWindow->setMaximumSize(QSize(500, 580));
        InstallerWindow->setStyleSheet(QString::fromUtf8(""));
        centralwidget = new QWidget(InstallerWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Plain);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        headerImage = new QLabel(frame);
        headerImage->setObjectName(QString::fromUtf8("headerImage"));
        headerImage->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(headerImage);

        belowHeaderColumns = new QHBoxLayout();
        belowHeaderColumns->setSpacing(20);
        belowHeaderColumns->setObjectName(QString::fromUtf8("belowHeaderColumns"));
        belowHeaderColumns->setContentsMargins(20, 0, 20, 20);
        leftImage = new QLabel(frame);
        leftImage->setObjectName(QString::fromUtf8("leftImage"));

        belowHeaderColumns->addWidget(leftImage);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(16);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, -1, -1);
        stackedWidget = new QStackedWidget(frame);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));

        verticalLayout_2->addWidget(stackedWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(8);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        backButton = new QPushButton(frame);
        backButton->setObjectName(QString::fromUtf8("backButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(backButton->sizePolicy().hasHeightForWidth());
        backButton->setSizePolicy(sizePolicy1);
        backButton->setMinimumSize(QSize(60, 0));

        horizontalLayout->addWidget(backButton);

        nextButton = new QPushButton(frame);
        nextButton->setObjectName(QString::fromUtf8("nextButton"));
        sizePolicy1.setHeightForWidth(nextButton->sizePolicy().hasHeightForWidth());
        nextButton->setSizePolicy(sizePolicy1);
        nextButton->setMinimumSize(QSize(60, 0));

        horizontalLayout->addWidget(nextButton);

        horizontalSpacer_2 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        cancelButton = new QPushButton(frame);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        sizePolicy1.setHeightForWidth(cancelButton->sizePolicy().hasHeightForWidth());
        cancelButton->setSizePolicy(sizePolicy1);
        cancelButton->setMinimumSize(QSize(60, 0));

        horizontalLayout->addWidget(cancelButton);


        verticalLayout_2->addLayout(horizontalLayout);


        belowHeaderColumns->addLayout(verticalLayout_2);

        rightImage = new QLabel(frame);
        rightImage->setObjectName(QString::fromUtf8("rightImage"));

        belowHeaderColumns->addWidget(rightImage);


        verticalLayout->addLayout(belowHeaderColumns);


        gridLayout->addWidget(frame, 0, 0, 1, 1);

        InstallerWindow->setCentralWidget(centralwidget);

        retranslateUi(InstallerWindow);

        QMetaObject::connectSlotsByName(InstallerWindow);
    } // setupUi

    void retranslateUi(QMainWindow *InstallerWindow)
    {
        InstallerWindow->setWindowTitle(QCoreApplication::translate("InstallerWindow", "noidget", nullptr));
        headerImage->setText(QString());
        leftImage->setText(QString());
        backButton->setText(QCoreApplication::translate("InstallerWindow", "Back", nullptr));
        nextButton->setText(QCoreApplication::translate("InstallerWindow", "Next", nullptr));
        cancelButton->setText(QCoreApplication::translate("InstallerWindow", "Cancel", nullptr));
        rightImage->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class InstallerWindow: public Ui_InstallerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INSTALLERWINDOW_H
