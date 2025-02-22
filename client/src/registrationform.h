#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QWidget>
#include <QString>

#include "chatclient.h"

namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationForm(QWidget *parent = nullptr);
    ~RegistrationForm();
    void setServer(std::shared_ptr<ChatClient> dbPtr);

signals:
    void loginRequested();
    //std::shared_ptr<ChatClient> dbPtr,
    void accepted(const QString &userName, int userId);
    void rejected();

private slots:
    void on_loginButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    //получениe ответа ot сервера
    void onServerResponseReceived(const QString &response);
    //обработка ошибки
    void onConnectionError(const QString &errorMessage);

private:
    Ui::RegistrationForm *ui;

    // Создание экземпляра ChatClient
    std::shared_ptr<ChatClient> m_dbPtr;
    QString m_userName;
    int m_userId;

    //кодировка пароля
    QString pwdEncoding(short n);
    bool validateLogin(const QString &login, const QString &password, const QString &confirmPas);
};

#endif // REGISTRATIONFORM_H
