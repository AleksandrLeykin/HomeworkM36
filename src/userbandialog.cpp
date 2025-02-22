#include "userbandialog.h"

UserBanDialog::UserBanDialog(QWidget *parent) : QDialog(parent) {
    userListWidget = new QListWidget(this);
    banButton = new QPushButton("Ban User", this);
    unblockButton = new QPushButton("unblock client", this);

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

