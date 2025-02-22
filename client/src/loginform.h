#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QString>

#include "chatclient.h"

namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

    void setServer(std::shared_ptr<ChatClient> dbPtr);

signals:
    void registrRequested();
    //передаю имя и id
    //std::shared_ptr<ChatClient> dbPtr,
    void accepted(const QString &userName, int userId);
    void rejected();

private slots:

    void on_registrationButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    //получениe ответа ot сервера
    void onServerResponseReceived(const QString &response);
    //обработка ошибки
    void onConnectionError(const QString &errorMessage);

private:
    Ui::LoginForm *ui;
    // Создание экземпляра ChatClient
    std::shared_ptr<ChatClient> m_dbPtr;

    QString m_userName;
    int m_userId;

    //кодировка пароля
    QString pwdEncoding();
    bool validateLogin(const QString &login, const QString &password);
};

#endif // LOGINFORM_H
