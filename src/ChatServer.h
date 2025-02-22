#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QMessageBox>
#include <QMap>
#include <QDateTime>

class ChatServer : public QTcpServer {
    Q_OBJECT
public:
    ChatServer(QObject *parent = nullptr) : QTcpServer(parent) {
         //connect(this, &QTcpServer::newConnection, this, &ChatServer::onNewConnection);
    }

    ~ChatServer() {
        for(QTcpSocket *clientSocket : m_users.keys()){
            clientSocket->disconnectFromHost(); // Отключаем клиента
            //clientSocket->deleteLater(); // Помечаем для удаления
        }
        clients.clear(); // Очищаем список клиентов
        m_users.clear();
    }

    ChatServer(const ChatServer&) = delete; // Запретить копирование
    ChatServer& operator=(const ChatServer&) = delete; // Запретить присваивание
    ChatServer(ChatServer&&) = delete;//default; // Разрешить перемещение
    ChatServer& operator=(ChatServer&&) = delete;//default; // Разрешить перемещение

    void startServer(quint16 port);
    void closeServer();

private slots:
    //новое подключение new connection
    void onNewConnection();
    //прием reception
    void readMessage();
    //запрос зарегистрированных пользователей Request from registered users
    void loadRegisteredUsers();
    // обработка отключения клиента
    void clientDisconnected();

private:
    // Здесь хранятся все зарегистрированные клиенты All registered clients are stored here.
    QMap<QString, QString> clients;
    // Здесь хранятся все подключенные клиенты All connected clients are stored here.
    QMap<QTcpSocket*, QString> m_users;

    //вход entrance
    bool userLogin(const QString &username, const QString &password, QTcpSocket* clientSocket);

    //регистрация registration
    bool registerUser(const QString &username, const QString &password, QTcpSocket* clientSocket);

    //отправка
    void sendMessage(const QString &sender, const QString &receiver, const QString &message);
    void sendMessageToUser(const QString& name, const QString &senderName, const QString& message);

    //время time
    QString logCurrentDateTime();
};

#endif // CHATSERVER_H
