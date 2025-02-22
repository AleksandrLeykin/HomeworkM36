#ifndef USERNAMEREGISTED_H
#define USERNAMEREGISTED_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>

class UserNameRegistedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserNameRegistedDialog(QWidget *parent = nullptr);

    //вывод списка пользователей
    void setUserLists(const QStringList &users);

    //получения выбранного пользователя
    QString getSelectedUser() const;

signals:
    //void sendPrivateMessage(const QString &userName, const QString &message);
    //void sendMessage();

private slots:
    void on_sendPrivateButton_clicked();

    void on_sendButton_clicked();

private:
    QListWidget *userListWidget;
    QPushButton *sendPrivateButton;
    QPushButton *sendButton;
};

#endif // USERNAMEREGISTED_H
