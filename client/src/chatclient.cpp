#include "chatclient.h"

ChatClient::ChatClient(const QString &host, quint16 port, QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this)), m_connected(false)
    {
    connect(socket, &QTcpSocket::connected, this, &ChatClient::onConnected);
    // Подключаем сигнал readyRead к слоту
    connect(socket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ChatClient::onDisconnected);


    socket->connectToHost(host, port);
    if(!socket->waitForConnected(3000)) {// Ждем 3 секунды
        emit connectionError("ERROR server connected!");
    }
}

void ChatClient::sendMessage(const QString &message, int userId) {
    // Проверка, существует ли пользователь с данным идентификатором
    if(!m_users.contains(userId)) {
        emit connectionError(QString("User not found: %1").arg(userId));
        return;
    }

    if(m_connected) {
        socket->write(message.toUtf8());
    }
    else
        emit connectionError("ERROR send message!");
}

void ChatClient::sendMessageRegistred(const QString &message)
{
    if(m_connected) {
        socket->write(message.toUtf8());
    }
    else
        emit connectionError("ERROR send message!");
}

void ChatClient::onConnected(){
    //qDebug() << "Connected to server!";
    m_connected = true;
}

void ChatClient::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "Received:" << data;
    // обработka полученныx данныx
    QString response = QString::fromUtf8(data);

    if(response.startsWith("Chat login successful") ||
        response.startsWith("Invalid login format\n") ||
        response.startsWith("Chat login failed\n")) {
        emit loginResponseReceived(response);
    }
    else if(response.startsWith("Registration successful") ||
            response.startsWith("Registration failed\n") ||
            response.startsWith("Invalid registration format\n")) {
        emit registrationResponseReceived(response);
    }
    // Передача данных в слот
    emit responseReceived(response);
}

void ChatClient::onDisconnected() {
    qDebug() << "Disconnected from server!";
    m_connected = false;
}

QString ChatClient::prepareMessage(const QString &message, int userId) {
    return QString("SEND_MESSAGE:%1:%2").arg(userId).arg(message);
}

void ChatClient::addUser(int id, const QString &name) {
    m_users.insert(id, User(id, name));
}

bool ChatClient::isConnected() {
    return m_connected;
}

