/********************************************************************************
** Form generated from reading UI file 'qrcodedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QRCODEDIALOG_H
#define UI_QRCODEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QRCodeDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QLabel *lblQRCode;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QCheckBox *chkReqPayment;
    QHBoxLayout *horizontalLayout;
    QLabel *lblAmount;
    QLineEdit *lnReqAmount;
    QLabel *lblBTC;
    QGridLayout *gridLayout;
    QLabel *lblLabel;
    QLineEdit *lnLabel;
    QLabel *lblMessage;
    QLineEdit *lnMessage;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSaveAs;

    void setupUi(QDialog *QRCodeDialog)
    {
        if (QRCodeDialog->objectName().isEmpty())
            QRCodeDialog->setObjectName(QStringLiteral("QRCodeDialog"));
        QRCodeDialog->resize(320, 404);
        verticalLayout_3 = new QVBoxLayout(QRCodeDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        lblQRCode = new QLabel(QRCodeDialog);
        lblQRCode->setObjectName(QStringLiteral("lblQRCode"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblQRCode->sizePolicy().hasHeightForWidth());
        lblQRCode->setSizePolicy(sizePolicy);
        lblQRCode->setMinimumSize(QSize(300, 300));
        lblQRCode->setAlignment(Qt::AlignCenter);
        lblQRCode->setWordWrap(true);

        verticalLayout_3->addWidget(lblQRCode);

        widget = new QWidget(QRCodeDialog);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        chkReqPayment = new QCheckBox(widget);
        chkReqPayment->setObjectName(QStringLiteral("chkReqPayment"));
        chkReqPayment->setEnabled(true);

        verticalLayout->addWidget(chkReqPayment);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lblAmount = new QLabel(widget);
        lblAmount->setObjectName(QStringLiteral("lblAmount"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblAmount->sizePolicy().hasHeightForWidth());
        lblAmount->setSizePolicy(sizePolicy1);
        lblAmount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(lblAmount);

        lnReqAmount = new QLineEdit(widget);
        lnReqAmount->setObjectName(QStringLiteral("lnReqAmount"));
        lnReqAmount->setEnabled(false);
        lnReqAmount->setMinimumSize(QSize(60, 0));
        lnReqAmount->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(lnReqAmount);

        lblBTC = new QLabel(widget);
        lblBTC->setObjectName(QStringLiteral("lblBTC"));
        sizePolicy1.setHeightForWidth(lblBTC->sizePolicy().hasHeightForWidth());
        lblBTC->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(lblBTC);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        lblLabel = new QLabel(widget);
        lblLabel->setObjectName(QStringLiteral("lblLabel"));
        lblLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lblLabel, 0, 0, 1, 1);

        lnLabel = new QLineEdit(widget);
        lnLabel->setObjectName(QStringLiteral("lnLabel"));
        lnLabel->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(lnLabel, 0, 1, 1, 1);

        lblMessage = new QLabel(widget);
        lblMessage->setObjectName(QStringLiteral("lblMessage"));
        lblMessage->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(lblMessage, 1, 0, 1, 1);

        lnMessage = new QLineEdit(widget);
        lnMessage->setObjectName(QStringLiteral("lnMessage"));
        lnMessage->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(lnMessage, 1, 1, 1, 1);


        horizontalLayout_2->addLayout(gridLayout);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        btnSaveAs = new QPushButton(widget);
        btnSaveAs->setObjectName(QStringLiteral("btnSaveAs"));

        horizontalLayout_3->addWidget(btnSaveAs);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout_3->addWidget(widget);

#ifndef QT_NO_SHORTCUT
        lblAmount->setBuddy(lnReqAmount);
        lblBTC->setBuddy(lnReqAmount);
        lblLabel->setBuddy(lnLabel);
        lblMessage->setBuddy(lnMessage);
#endif // QT_NO_SHORTCUT

        retranslateUi(QRCodeDialog);
        QObject::connect(chkReqPayment, SIGNAL(clicked(bool)), lnReqAmount, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(QRCodeDialog);
    } // setupUi

    void retranslateUi(QDialog *QRCodeDialog)
    {
        QRCodeDialog->setWindowTitle(QApplication::translate("QRCodeDialog", "Dialog", Q_NULLPTR));
        lblQRCode->setText(QApplication::translate("QRCodeDialog", "QR Code", Q_NULLPTR));
        chkReqPayment->setText(QApplication::translate("QRCodeDialog", "Request Payment", Q_NULLPTR));
        lblAmount->setText(QApplication::translate("QRCodeDialog", "Amount:", Q_NULLPTR));
        lblBTC->setText(QApplication::translate("QRCodeDialog", "LMC", Q_NULLPTR));
        lblLabel->setText(QApplication::translate("QRCodeDialog", "Label:", Q_NULLPTR));
        lblMessage->setText(QApplication::translate("QRCodeDialog", "Message:", Q_NULLPTR));
        btnSaveAs->setText(QApplication::translate("QRCodeDialog", "&Save As...", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QRCodeDialog: public Ui_QRCodeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QRCODEDIALOG_H
