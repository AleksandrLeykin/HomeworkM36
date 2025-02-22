#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startscreeen.h"
#include "usernameregisted.h"
#include <QTimer>
#include <QStringList>
#include <QMessageBox>
#include <QDateTime>

int MainWindow::InstanceCountClient = 0;

MainWindow::MainWindow(std::shared_ptr<ChatClient> chatPtr, QString userName, int userId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_userName(userName)
    , m_userId(userId)    
{
    ui->setupUi(this);
    // Устанавливаем заголовок окна
    setWindowTitle("ChatClient");
    InstanceCountClient++;
    if(chatPtr)
        m_chatPtr = chatPtr;
    else
        m_chatPtr = std::make_shared<ChatClient>("127.0.0.1", 7777);

    //подключение сигнала о  сообщения на сервере
    connect(m_chatPtr.get(), &ChatClient::responseReceived, [this](const QString &message) {
        handleMessage(message);
    });
}

MainWindow::~MainWindow() {
    delete ui;
    InstanceCountClient--;
    if(InstanceCountClient <= 0)
        qApp->exit(0);
}

MainWindow *MainWindow::createClient() {
    //запрос логина и пароля через новые формы
    StartScreeen s;
    auto result = s.exec();
    if(result == QDialog::Rejected) {
        return nullptr;
    }
    //открытие нового окна
    auto w = new MainWindow(s.getDbptr(), s.userName(), s.userId());

    //qDebug() << "MainWindow: " << w->m_userName;
    //qDebug() << "MainWindow: " << w->m_userId;
    w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

void MainWindow::on_actionConnecting_a_new_client_triggered() {
    auto w = createClient();
    if(w)
        w->show();
}

void MainWindow::on_actionClose_client_triggered() {
    this->close();
}

void MainWindow::handleMessage(const QString &message) {
    //qDebug() << "Received message:" << message;
    //обработка приема ответа сервера
    if (message.startsWith("USERS")) {
        m_users = message.mid(6).split(':');
        // Проверка на пустой список
        if (m_users.isEmpty()) {
            qDebug() << "MainWindow: No users found.";
            return; // Выход, если нет пользователей
        }
        QString name = openDialogListUser(m_users);
        if (!name.isEmpty() && !ui->messageLineEdit->text().isEmpty()) {
            QString messadeSend = ui->messageLineEdit->text();
            sendMessageToUser(name, messadeSend);
            //qDebug() << "MainWindow: message for - " << name;
            //qDebug() << "MainWindow: message from - " << m_userName;
            //qDebug() << "MainWindow: message: " << messadeSend;
        }
    }
    else if (message.startsWith("MESSAGE TO YOU")) {
        QStringList messageToYou = message.mid(15).split('\n');
        if(messageToYou.isEmpty()) {
            qDebug() << "MainWindow: No message for you!";
            ui->privateChatTextBrowser->append(" No message for you!");
        }
        else {
            for(const QString &message : messageToYou)
                ui->privateChatTextBrowser->append(message);
        }
    }
    else if (message.startsWith(m_userName.toUtf8())) {
        QStringList parts = message.split(":");
        if(parts.size() >= 3){
            QString youMessage = logCurrentDateTime() + " " + parts.mid(1).join(" ");
            ui->commonChatTextBrowser->append(youMessage.toUtf8());
        }
    }
}

void MainWindow::sendMessageToUser(const QString &userName, const QString &message) {
    //qDebug() << "Sending message to user:" << userName << "Message:" << message;
    QString messageToSend = QString("MESSAGE TO:%1:%2:%3").arg(userName, m_userName, message);
    if(!ui->messageLineEdit->text().isEmpty()) {
        m_chatPtr->sendMessageRegistred(messageToSend);
        ui->messageLineEdit->clear();
    }
}

QString MainWindow::openDialogListUser(QStringList list) {
    //обработка открытия окна для выбора пользователя
    UserNameRegistedDialog dialog(this);
    dialog.setUserLists(list);
    // Запуск диалога
    dialog.exec();
    //Выбор пользователя для сообщения
    QString userIdName = dialog.getSelectedUser();
    QString name;
    //отправка непосредственно сообщения
    if(!userIdName.isEmpty()) {
        //отделяю id от name
        int firstSpaceIndex = userIdName.indexOf(' ');

        if(firstSpaceIndex != - 1)
            name = userIdName.mid(firstSpaceIndex + 1).trimmed();
    }
    return name;
}

void MainWindow::handleSendMessage() {
    // Логика для общего сообщения
    if(m_chatPtr->isConnected()){
        QString messageSend ="ALL SEND TO:" + m_userName + ":" + ui->messageLineEdit->text();
        if(!ui->messageLineEdit->text().isEmpty()) {
            m_chatPtr->sendMessageRegistred(messageSend);
            ui->messageLineEdit->clear();
        }
    }
}

void MainWindow::on_sendMessageButton_clicked() {
    if(ui->messageLineEdit->text().isEmpty()) {
       QMessageBox::warning(this, "ATTENTION!!", "ENTER MESSAGE!!");
    }
    else {
        handleSendMessage();
    }
    if(!m_chatPtr->isConnected()) {
        ui->commonChatTextBrowser->append("Not connect to server!");
    }
}

void MainWindow::openUserNameregistedDialog() {
    if(m_chatPtr->isConnected()) {
        //подготовка и передача сообщения
        QString setUsers = "SET USERS NAME";
        m_chatPtr->sendMessageRegistred(setUsers);
    }
    else {
        qDebug() << "Not connect to server!";
    }
}

void MainWindow::on_privateMessageButton_clicked() {
    if(ui->messageLineEdit->text().isEmpty())
        QMessageBox::warning(this, "ATTENTION!!", "ENTER MESSAGE!!");
    else
        openUserNameregistedDialog();
    if(!m_chatPtr->isConnected()) {
        ui->commonChatTextBrowser->append("Not connect to server!");
    }
}

void MainWindow::on_messageLineEdit_returnPressed() {
    on_privateMessageButton_clicked();
    on_sendMessageButton_clicked();
}

void MainWindow::on_messageForMeButton_clicked(){
    ui->privateChatTextBrowser->clear();
    openPrivateMessageList();
}

void MainWindow::openPrivateMessageList() {
    if(m_chatPtr->isConnected()) {
        //подготовка и передача сообщения
        QString setMessage = "SHOW MESSAGE FOR ME:" +
                             QString::number(m_userId) + ":" + m_userName;
        m_chatPtr->sendMessageRegistred(setMessage);
    }
    else {
        ui->commonChatTextBrowser->append("Not connect to server!");
        //qDebug() << "Not connect to server!";
    }
}

QString MainWindow::logCurrentDateTime() {
    // Получаем текущее время и дату
    QDateTime current = QDateTime::currentDateTime();
    // Форматируем дату и время в строку
    QString formattedDateTime = current.toString("yyyy-MM-dd HH:mm");
    return formattedDateTime;
}

