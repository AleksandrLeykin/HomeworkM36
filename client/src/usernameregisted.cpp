#include "usernameregisted.h"

#include <QMessageBox>

UserNameRegistedDialog::UserNameRegistedDialog(QWidget *parent) :QDialog(parent) {
    // Устанавливаем заголовок окна
    setWindowTitle("User Lists");

    userListWidget = new QListWidget(this);
    sendPrivateButton = new QPushButton("Private message", this);
    sendButton = new QPushButton("Send message all", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(userListWidget);
    layout->addWidget(sendPrivateButton);
    layout->addWidget(sendButton);

    connect(sendPrivateButton, &QPushButton::clicked, this, &UserNameRegistedDialog::on_sendPrivateButton_clicked);
    connect(sendButton, &QPushButton::clicked, this, &UserNameRegistedDialog::on_sendButton_clicked);
}

void UserNameRegistedDialog::setUserLists(const QStringList &users) {
    userListWidget->clear();
    userListWidget->addItems(users);
}

QString UserNameRegistedDialog::getSelectedUser() const {
    // Возвращает текущий текст (имя выбранного пользователя)
    QListWidgetItem *selectedItem = userListWidget->currentItem();

    return selectedItem ? selectedItem->text() : QString();
}

void UserNameRegistedDialog::on_sendPrivateButton_clicked() {
    QString selectedUser = getSelectedUser();
    QString message;
    if(!selectedUser.isEmpty()) {
        //emit sendPrivateMessage(selectedUser, message);
        accept();
    }
    else {
        QMessageBox::warning(this, "Warning", "Please select a user to send a private message.");
    }
}

void UserNameRegistedDialog::on_sendButton_clicked() {
    QString selectedUser = userListWidget->currentItem()->text();
    //emit sendMessage();
    accept();
}
