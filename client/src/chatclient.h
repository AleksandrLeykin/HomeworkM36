#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QMap>

#include "User.h"

class ChatClient : public QObject {
     Q_OBJECT
public:
    ChatClient(const QString &host, quint16 port, QObject *parent = nullptr);
    ~ChatClient() = default;

    void sendMessage(const QString &message, int id);
    void sendMessageRegistred(const QString &message);

    // Метод для добавления пользователя
    void addUser(int id, const QString &name);
    //проверка подключения сервера
    bool isConnected();

    int m_id = 0;
    QString m_userName;

signals:
    void responseReceived(const QString &response);
    void connectionError(const QString &errorMessage);

    //сигналы для отправки в LoginForm, RegistrationForm
    void loginResponseReceived(const QString &response);
    void registrationResponseReceived(const QString &response);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *socket;

    bool m_connected;

    // Container for storing users, для хранения клиентов
    QMap<int, User> m_users;

    // Подготовка сообщения
    QString prepareMessage(const QString &message, int userId);
};

#endif // CHATCLIENT_H
