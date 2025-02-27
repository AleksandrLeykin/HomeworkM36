#include "ChatServer.h"
#include "mySQLTest.h"



ChatServer::ChatServer(QObject *parent) : QTcpServer(parent) { }

ChatServer::~ChatServer() {
    for(QTcpSocket *clientSocket : m_users.keys()){
        // Отключаем клиента
        clientSocket->disconnectFromHost();
    }
    // Очищаем список клиентов
    clients.clear();
    m_users.clear();
}

void ChatServer::startServer(quint16 port) {
    if (!this->listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << logCurrentDateTime() << "Server listen....";
        connect(this, &QTcpServer::newConnection, this, &ChatServer::onNewConnection);
        loadRegisteredUsers();
    }
}

void ChatServer::onNewConnection() {
    //qDebug() << "ChatServer: onNewConnection start!";
    // новый сокет для подключения
    QTcpSocket* clientSocket = this->nextPendingConnection();

    if(clientSocket) {
        connect(clientSocket, &QTcpSocket::readyRead, this, &ChatServer::readMessage);
        // блокировка повторного подключения сигнала отключения
        disconnect(clientSocket, &QTcpSocket::disconnected, this, &ChatServer::clientDisconnected);
        // отключение пользователя
        connect(clientSocket, &QTcpSocket::disconnected, this, &ChatServer::clientDisconnected);        
    }
    else {
        qDebug() << "Failed to create socket!";
        return;
    }

}

void ChatServer::clientDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if(clientSocket) {
        // Получаем имя пользователя
        QString userName = m_users.value(clientSocket);
        qDebug() << logCurrentDateTime() << " user disconnected - " << userName;
        // Удаляем клиента из списка
        m_users.remove(clientSocket);
        // Удаляем сокет
        clientSocket->deleteLater();
    }
}

bool ChatServer::registerUser(const QString &username, const QString &password, QTcpSocket* clientSocket) {
    // Проверка на валидность имени пользователя и пароля
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "Username or password cannot be empty.";
        return false;
    }
    // подключаемся к БД
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    if(mdb->addingUser(username, password)) {
        // Сохранение пользователя в контейнер
        clients.insert(username, password);
        m_users.insert(clientSocket, username);
        qDebug() << logCurrentDateTime() << "User registered:" << username;
        return true;
    }
    else
        return false;
}

bool ChatServer::userLogin(const QString &username, const QString &password, QTcpSocket* clientSocket) {    
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "Username or password cannot be empty.";
        return false;
    }

    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    if(mdb->userLogin(username, password)) {
        qDebug() << logCurrentDateTime() << "User connected:" << username;
        m_users.insert(clientSocket, username);
        return true;
    }
    else
        return false;
}

//sender - отправитель, receiver - получатель
void ChatServer::sendMessage(const QString &sender, const QString &receiver,
                             const QString &message) {    
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    if(mdb->writingMessage(sender, receiver, message))
        qDebug() << logCurrentDateTime() << "message delivered" << receiver;
    else
        qDebug() << "sendMessage(): message not delivered";
    //Отправка сообщения получателю
    for (QTcpSocket *client : m_users.keys()) {
        if (m_users[client] == receiver) {
            client->write(QString("%1: %2\n").arg(sender).arg(message).toUtf8());
            break;
        }
    }
}

void ChatServer::sendMessageToUser(const QString &name, const QString &senderName, const QString &message) {
    QTcpSocket* targetSocket = nullptr;
    // Ищем сокет, соответствующий имени
    for(auto it = m_users.begin(); it != m_users.end(); ++it) {
        if (it.value() == name) {
            targetSocket = it.key();
            break;
        }
    }
    if(targetSocket) {
        QString messageName = name + ":" + senderName + ":" + message;
        targetSocket->write(messageName.toUtf8());
        qDebug() << logCurrentDateTime() << "Message sent to user: " << name;
    }
    else {
        qDebug() << logCurrentDateTime() << "user to name: " << name << " not found.";
    }
}

void ChatServer::closeServer() {
    for(QTcpSocket *clientSocket : m_users.keys()){
        // Отключение клиента
        clientSocket->disconnectFromHost();
    }
    clients.clear();
    m_users.clear();
}

void ChatServer::readMessage() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        qDebug() << "Invalid socket!";
        return;
    }
    QByteArray data = clientSocket->readAll();

    if (data.isEmpty()) {
        qDebug() << "No data received!";
        return;
    }

    QString message(data);
    qDebug() << logCurrentDateTime() << message;
    // сообщение начинается с "REGISTER", то это команда регистрации
    if (message.startsWith("REGISTER")) {
        QStringList parts = message.split(":");
        if (parts.size() == 3) {
            QString username = parts[1];
            QString password = parts[2];
            if (registerUser(username, password, clientSocket)) {
                //если зарегистрирован то возвращаем имя и id
                std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
                QString userID = mdb->idName(username, password);
                QString result = QString("Registration successful:%1:%2").arg(username,userID);
                clientSocket->write(result.toUtf8());
            } else {
                clientSocket->write("Registration failed\n");
            }
        } else {
            clientSocket->write("Invalid registration format\n");
        }
    }
    // сообщение начинается с "Login" то это команда входа
    else if(message.startsWith("Login")) {
        QStringList parts = message.split(":");
        if (parts.size() == 3) {
            QString username = parts[1];
            QString password = parts[2];
            if (userLogin(username, password, clientSocket)) {
                //если зарегистрирован то возвращаем имя и id
                std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
                QString userID = mdb->idName(username, password);
                //запрос
                QString result = QString("Chat login successful:%1:%2").arg(username,userID);
                clientSocket->write(result.toUtf8());
            } else {
                clientSocket->write("Chat login failed\n");
            }
        } else {
            clientSocket->write("Invalid login format\n");
        }
    }
    // сообщение начинается с "SEND", то это команда отправки сообщения
    else if (message.startsWith("SEND")) {
        QStringList parts = message.split(" ", Qt::SkipEmptyParts);
         // >=, чтобы учесть возможность большего количества частей
        if (parts.size() >= 3) {
            QString receiver = parts[1];
            // Объединие оставшийся части в одно сообщение
            QString msg = parts.mid(2).join(" ");
            sendMessage(m_users[clientSocket], receiver, msg);
        } else {
            clientSocket->write("SEND: Invalid send format\n");
        }
    }
    // сообщение начинается с "SET USERS NAME" команда на передачу имен пользователей
    else if (message.startsWith("SET USERS NAME")) {
        std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
        QString usersNames = mdb->userRegistrftion();
        clientSocket->write(usersNames.toUtf8());
    }
    // сообщение начинается с "MESSAGE TO" передача сообщения определленному пользователю
    else if (message.startsWith("MESSAGE TO")) {
        QStringList parts = message.split(":");
        if (parts.size() >= 4) {
            //получатель - recipient, отправитель - sender
            QString recipientName = parts[1];
            QString senderName = parts[2];
            QString transmittedMessage = parts.mid(3).join(" ");
            //запись сообщения пользователя
            std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
            if (mdb->writingMessage(recipientName, senderName, transmittedMessage)) {
                qDebug() << logCurrentDateTime() << "Message to " << recipientName
                         << " from " << senderName << " recorded!";
                clientSocket->write("message delivered!!");
                //если клиент в сети
                for(auto it = m_users.begin(); it != m_users.end(); ++it) {
                    if(it.value() == recipientName) {
                        sendMessageToUser(recipientName, senderName, transmittedMessage);
                        break;
                    }
                }
            }
            else
                qDebug() << "Message not recorded!";
        }        
    }
    // сообщение начинается с "SEND TO ALL" передача сообщения всем
    else if (message.startsWith("ALL SEND TO")) {
        QStringList parts = message.split(":");
        if(parts.size() >= 3) {
            QString senderName = parts[1];
            QString transmittedMessage = parts.mid(2).join(" ");

            std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
            if(mdb->writingMessageAll(senderName, transmittedMessage)) {
                qDebug() << logCurrentDateTime() << "Message to ALL delivered";
                clientSocket->write("message delivered!!");
            }
            //если клиент в сети
            for(auto it = m_users.begin(); it != m_users.end(); ++it) {
                sendMessageToUser(it.value(), senderName, transmittedMessage);
            }
        }
    }
    // сообщение начинается с "SHOW MESSAGE FOR ME" показ старых сообщений
    else if (message.startsWith("SHOW MESSAGE FOR ME")) {
        QStringList parts = message.split(':');
        QString userMessages = "MESSAGE TO YOU\n";
        if (parts.size() >= 3) {
            bool ok;
            int id = parts[1].toInt(&ok);
            if(ok) {
                QString userName = parts[2];
                std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
                userMessages += mdb->userViewMessages(id);
                clientSocket->write(userMessages.toUtf8());
                qDebug() << logCurrentDateTime() << "message transfer request completed" << userName;
            }
        }
    }
}

void ChatServer::loadRegisteredUsers() {
    // подключаемся к БД
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    QString users = mdb->loadRegistretedUsers();    
    QStringList pairs = users.split('\n');
    for(const QString &pair : pairs) {
        // Разделяем каждую пару по ':'
        QStringList userPassword = pair.split(':');
        if (userPassword.size() == 2) {
            QString username = userPassword[0].trimmed();
            QString password = userPassword[1].trimmed();
            clients.insert(username, password);
        }
    }
}

QString ChatServer::logCurrentDateTime() {
    // Получаем текущее время и дату
    QDateTime current = QDateTime::currentDateTime();
    // Форматируем дату и время в строку
    QString formattedDateTime = current.toString("yyyy-MM-dd HH:mm");
    return formattedDateTime;
}

