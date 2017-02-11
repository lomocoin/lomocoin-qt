/********************************************************************************
** Form generated from reading UI file 'messagepage.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEPAGE_H
#define UI_MESSAGEPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qvalidatedlineedit.h"

QT_BEGIN_NAMESPACE

class Ui_MessagePage
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelExplanation;
    QHBoxLayout *horizontalLayout_3;
    QValidatedLineEdit *signFrom;
    QPushButton *addressBookButton;
    QPushButton *pasteButton;
    QPlainTextEdit *message;
    QLineEdit *signature;
    QHBoxLayout *horizontalLayout;
    QPushButton *signMessage;
    QPushButton *copyToClipboard;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *MessagePage)
    {
        if (MessagePage->objectName().isEmpty())
            MessagePage->setObjectName(QStringLiteral("MessagePage"));
        MessagePage->resize(627, 380);
        verticalLayout = new QVBoxLayout(MessagePage);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        labelExplanation = new QLabel(MessagePage);
        labelExplanation->setObjectName(QStringLiteral("labelExplanation"));
        labelExplanation->setTextFormat(Qt::AutoText);
        labelExplanation->setWordWrap(true);

        verticalLayout->addWidget(labelExplanation);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        signFrom = new QValidatedLineEdit(MessagePage);
        signFrom->setObjectName(QStringLiteral("signFrom"));
        signFrom->setMaxLength(34);

        horizontalLayout_3->addWidget(signFrom);

        addressBookButton = new QPushButton(MessagePage);
        addressBookButton->setObjectName(QStringLiteral("addressBookButton"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/address-book"), QSize(), QIcon::Normal, QIcon::Off);
        addressBookButton->setIcon(icon);
        addressBookButton->setAutoDefault(false);
        addressBookButton->setFlat(false);

        horizontalLayout_3->addWidget(addressBookButton);

        pasteButton = new QPushButton(MessagePage);
        pasteButton->setObjectName(QStringLiteral("pasteButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/editpaste"), QSize(), QIcon::Normal, QIcon::Off);
        pasteButton->setIcon(icon1);
        pasteButton->setAutoDefault(false);

        horizontalLayout_3->addWidget(pasteButton);


        verticalLayout->addLayout(horizontalLayout_3);

        message = new QPlainTextEdit(MessagePage);
        message->setObjectName(QStringLiteral("message"));

        verticalLayout->addWidget(message);

        signature = new QLineEdit(MessagePage);
        signature->setObjectName(QStringLiteral("signature"));
        QFont font;
        font.setItalic(true);
        signature->setFont(font);
        signature->setReadOnly(true);

        verticalLayout->addWidget(signature);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        signMessage = new QPushButton(MessagePage);
        signMessage->setObjectName(QStringLiteral("signMessage"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/edit"), QSize(), QIcon::Normal, QIcon::Off);
        signMessage->setIcon(icon2);

        horizontalLayout->addWidget(signMessage);

        copyToClipboard = new QPushButton(MessagePage);
        copyToClipboard->setObjectName(QStringLiteral("copyToClipboard"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/editcopy"), QSize(), QIcon::Normal, QIcon::Off);
        copyToClipboard->setIcon(icon3);

        horizontalLayout->addWidget(copyToClipboard);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(MessagePage);

        QMetaObject::connectSlotsByName(MessagePage);
    } // setupUi

    void retranslateUi(QWidget *MessagePage)
    {
        MessagePage->setWindowTitle(QApplication::translate("MessagePage", "Message", Q_NULLPTR));
        labelExplanation->setText(QApplication::translate("MessagePage", "You can sign messages with your addresses to prove you own them. Be careful not to sign anything vague, as phishing attacks may try to trick you into signing your identity over to them. Only sign fully-detailed statements you agree to.", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        signFrom->setToolTip(QApplication::translate("MessagePage", "The address to sign the message with  (e.g. 1NS17iag9jJgTHD1VXjvLCEnZuQ3rJDE9L)", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        addressBookButton->setToolTip(QApplication::translate("MessagePage", "Choose adress from address book", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        addressBookButton->setText(QString());
        addressBookButton->setShortcut(QApplication::translate("MessagePage", "Alt+A", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        pasteButton->setToolTip(QApplication::translate("MessagePage", "Paste address from clipboard", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        pasteButton->setText(QString());
        pasteButton->setShortcut(QApplication::translate("MessagePage", "Alt+P", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        message->setToolTip(QApplication::translate("MessagePage", "Enter the message you want to sign here", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        signature->setText(QApplication::translate("MessagePage", "Click \"Sign Message\" to get signature", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        signMessage->setToolTip(QApplication::translate("MessagePage", "Sign a message to prove you own this address", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        signMessage->setText(QApplication::translate("MessagePage", "&Sign Message", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        copyToClipboard->setToolTip(QApplication::translate("MessagePage", "Copy the current signature to the system clipboard", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        copyToClipboard->setText(QApplication::translate("MessagePage", "&Copy to Clipboard", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MessagePage: public Ui_MessagePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEPAGE_H
