#ifndef STARTSCREEEN_H
#define STARTSCREEEN_H

#include <QDialog>
#include <memory>

#include "chatclient.h"

namespace Ui {
class StartScreeen;
}

class StartScreeen : public QDialog
{
    Q_OBJECT

public:
    explicit StartScreeen(std::shared_ptr<ChatClient> dbPtr = nullptr, QWidget *parent = nullptr);
    ~StartScreeen();

    void setLoginForm();
    void setRegistrationForm();

    //передача данных клиента
    int userId() const;
    QString userName() const;
    std::shared_ptr<ChatClient> getDbptr() const;

    //пробую закрыть форму временно!!
    void m_close();

public slots:
    //std::shared_ptr<ChatClient> dbPtr,
    void onLoggedIn(QString userName, uint userId);
    void onRejectRequested();

private:
    Ui::StartScreeen *ui;
    int m_userId;
    QString m_userName;
    //для подключения к серверу
    std::shared_ptr<ChatClient> m_dbPtr;
};

#endif // STARTSCREEEN_H
