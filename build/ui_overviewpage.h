/********************************************************************************
** Form generated from reading UI file 'overviewpage.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OVERVIEWPAGE_H
#define UI_OVERVIEWPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OverviewPage
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame;
    QFormLayout *formLayout_2;
    QLabel *label_5;
    QLabel *label;
    QLabel *labelBalance;
    QLabel *label_6;
    QLabel *labelStake;
    QLabel *label_3;
    QLabel *labelUnconfirmed;
    QLabel *label_7;
    QLabel *labelFrozen;
    QLabel *label_8;
    QLabel *labelMintingOnly;
    QLabel *label_2;
    QLabel *labelNumTransactions;
    QTableView *tableFrozenCoins;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout;
    QLabel *label_4;
    QListView *listTransactions;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *OverviewPage)
    {
        if (OverviewPage->objectName().isEmpty())
            OverviewPage->setObjectName(QStringLiteral("OverviewPage"));
        OverviewPage->resize(552, 464);
        horizontalLayout = new QHBoxLayout(OverviewPage);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        frame = new QFrame(OverviewPage);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        formLayout_2 = new QFormLayout(frame);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout_2->setHorizontalSpacing(12);
        formLayout_2->setVerticalSpacing(12);
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        label_5->setFont(font);

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_5);

        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label);

        labelBalance = new QLabel(frame);
        labelBalance->setObjectName(QStringLiteral("labelBalance"));
        labelBalance->setText(QStringLiteral("0 BTC"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, labelBalance);

        label_6 = new QLabel(frame);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_6);

        labelStake = new QLabel(frame);
        labelStake->setObjectName(QStringLiteral("labelStake"));
        labelStake->setText(QStringLiteral("0 BTC"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, labelStake);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_3);

        labelUnconfirmed = new QLabel(frame);
        labelUnconfirmed->setObjectName(QStringLiteral("labelUnconfirmed"));
        labelUnconfirmed->setText(QStringLiteral("0 BTC"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, labelUnconfirmed);

        label_7 = new QLabel(frame);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_7);

        labelFrozen = new QLabel(frame);
        labelFrozen->setObjectName(QStringLiteral("labelFrozen"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, labelFrozen);

        label_8 = new QLabel(frame);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout_2->setWidget(6, QFormLayout::LabelRole, label_8);

        labelMintingOnly = new QLabel(frame);
        labelMintingOnly->setObjectName(QStringLiteral("labelMintingOnly"));

        formLayout_2->setWidget(6, QFormLayout::FieldRole, labelMintingOnly);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout_2->setWidget(7, QFormLayout::LabelRole, label_2);

        labelNumTransactions = new QLabel(frame);
        labelNumTransactions->setObjectName(QStringLiteral("labelNumTransactions"));

        formLayout_2->setWidget(7, QFormLayout::FieldRole, labelNumTransactions);

        label->raise();
        labelBalance->raise();
        label_2->raise();
        label_3->raise();
        labelUnconfirmed->raise();
        label_6->raise();
        labelStake->raise();
        label_5->raise();
        labelNumTransactions->raise();
        label_7->raise();
        labelFrozen->raise();
        label_8->raise();
        labelMintingOnly->raise();

        verticalLayout_2->addWidget(frame);

        tableFrozenCoins = new QTableView(OverviewPage);
        tableFrozenCoins->setObjectName(QStringLiteral("tableFrozenCoins"));

        verticalLayout_2->addWidget(tableFrozenCoins);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        frame_2 = new QFrame(OverviewPage);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame_2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout->addWidget(label_4);

        listTransactions = new QListView(frame_2);
        listTransactions->setObjectName(QStringLiteral("listTransactions"));
        listTransactions->setFrameShape(QFrame::NoFrame);
        listTransactions->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listTransactions->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        verticalLayout->addWidget(listTransactions);


        verticalLayout_3->addWidget(frame_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout_3);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);

        retranslateUi(OverviewPage);

        QMetaObject::connectSlotsByName(OverviewPage);
    } // setupUi

    void retranslateUi(QWidget *OverviewPage)
    {
        OverviewPage->setWindowTitle(QApplication::translate("OverviewPage", "Form", Q_NULLPTR));
        label_5->setText(QApplication::translate("OverviewPage", "Wallet", Q_NULLPTR));
        label->setText(QApplication::translate("OverviewPage", "Balance:", Q_NULLPTR));
        label_6->setText(QApplication::translate("OverviewPage", "Stake:", Q_NULLPTR));
        label_3->setText(QApplication::translate("OverviewPage", "Unconfirmed:", Q_NULLPTR));
        label_7->setText(QApplication::translate("OverviewPage", "Frozen:", Q_NULLPTR));
        labelFrozen->setText(QApplication::translate("OverviewPage", "0 BTC", Q_NULLPTR));
        label_8->setText(QApplication::translate("OverviewPage", "MintingOnly:", Q_NULLPTR));
        labelMintingOnly->setText(QApplication::translate("OverviewPage", "0 BTC", Q_NULLPTR));
        label_2->setText(QApplication::translate("OverviewPage", "Number of transactions:", Q_NULLPTR));
        labelNumTransactions->setText(QApplication::translate("OverviewPage", "0", Q_NULLPTR));
        label_4->setText(QApplication::translate("OverviewPage", "<b>Recent transactions</b>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OverviewPage: public Ui_OverviewPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OVERVIEWPAGE_H
