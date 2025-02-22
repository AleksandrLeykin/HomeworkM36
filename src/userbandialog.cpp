#include "userbandialog.h"

UserBanDialog::UserBanDialog(QWidget *parent) : QDialog(parent) {
    this->setWindowTitle("Ban and unBlock");
    userListWidget = new QListWidget(this);
    banButton = new QPushButton("Ban User", this);
    unblockButton = new QPushButton("unblock client", this);


    my_styles();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(userListWidget);
    layout->addWidget(banButton);
    layout->addWidget(unblockButton);

    connect(banButton, &QPushButton::clicked, this, &UserBanDialog::on_BanButton_clicked);
    connect(unblockButton, &QPushButton::clicked, this, &UserBanDialog::on_unblockButton_clicked);
}

void UserBanDialog::setUserList(const QStringList &users) {
    userListWidget->addItems(users);
}

void UserBanDialog::on_BanButton_clicked() {
    QString selectedUser = userListWidget->currentItem()->text();
    emit userBanned(selectedUser);
    accept(); // Закрываем диалог
}
void UserBanDialog::on_unblockButton_clicked() {
    QString selectedUser = userListWidget->currentItem()->text();
    emit unblock(selectedUser);
    accept(); // Закрываем диалог
}

void UserBanDialog::my_styles()
{
    this->setStyleSheet("background-color: rgb(30, 100, 70);");

    banButton->setStyleSheet("QPushButton {"
                             "background-color: rgb(255, 0, 0);"
                             "color: white;"
                             "border: 1px solid black;"
                             "border-radius: 3px;"
                             "padding: 5px;"
                             "}"
                             "QPushButton:hover {"
                             "background-color: darkred;"
                             "}");
    unblockButton->setStyleSheet("QPushButton {"
                                 "background-color: green;"
                                 "color: white;"
                                 "border: 1px solid black;"
                                 "border-radius: 3px;"
                                 "padding: 5px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: yellow;"
                                 "color: black;"
                                 "}");
    userListWidget->setStyleSheet("QListWidget {color: yellow;}"
                                  "QListWidget:hover {color: orange;}");
}

