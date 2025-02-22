#include "loginform.h"
#include "ui_loginform.h"
#include "sha256.h"

#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginForm)
{
    ui->setupUi(this);   
}

LoginForm::~LoginForm() {
    delete ui;
}

void LoginForm::setServer(std::shared_ptr<ChatClient> dbPtr) {
    m_dbPtr = dbPtr;
    if(m_dbPtr->isConnected()) {
        ui->serverTextEdit->append("Connected to server");
    }
    else {
        ui->serverTextEdit->append("ERROR connected to server!");
    }
    //подключение сигналов от ChatClient
    connect(m_dbPtr.get(), &ChatClient::connectionError, this, &LoginForm::onConnectionError);
    connect(m_dbPtr.get(), &ChatClient::loginResponseReceived, this, &LoginForm::onServerResponseReceived);
}

void LoginForm::on_registrationButton_clicked() {
    emit registrRequested();
}

void LoginForm::on_buttonBox_accepted() {
    m_userName = ui->loginlineEdit->text();
    QString password = pwdEncoding();

    if(validateLogin(m_userName, password)) {
        if(m_dbPtr->isConnected()) {
        QString message = QString("Login:%1:%2").arg(m_userName).arg(password);
        m_dbPtr->sendMessageRegistred(message);
        }
        else {
            QMessageBox::warning(this, "ERROR", "No connection to the server. Please try again later.");
            return;
        }
    }
    else{
        //qDebug() << "ERROR registranion!";
        QMessageBox::warning(this, "ERROR", "Incorrect login or password. Please try again.");        
        ui->PasswordlineEdit->clear();
        ui->loginlineEdit->setFocus();
    }
}

void LoginForm::on_buttonBox_rejected() {
    emit rejected();
}

void LoginForm::onServerResponseReceived(const QString &response) {
    //при успешном входе получаем name and id
    if(response.startsWith("Chat login successful")) {
        QStringList parts = response.split(":");
        int number;
        if(parts.size() == 3) {            
            qDebug() << parts[1];
            bool convert; //переменнaя для проверки конверсии в число
            number = parts[2].toInt(&convert);
            if(convert) {               
                qDebug() << number;
            }
            else
                qDebug() << "ERROR userId not integer!";
        }
        // Успешный вход m_dbPtr,
        emit accepted(parts[1], number);
    }
    else {
        // Ошибка при входе
        QMessageBox::warning(this, "ERROR", "Name or password do not match. Please try again.");
        //ui->loginlineEdit->clear();
        ui->PasswordlineEdit->clear();
        ui->loginlineEdit->setFocus();
    }
}

void LoginForm::onConnectionError(const QString &errorMessage) {
    QMessageBox::warning(this, "Connection Error", errorMessage);
    // очистка полей ввода
    ui->loginlineEdit->clear();
    ui->PasswordlineEdit->clear();
    ui->loginlineEdit->setFocus();
}

QString LoginForm::pwdEncoding() {
    SHA256 sha256;
    if(!ui->PasswordlineEdit->text().isEmpty()) {
    QString pass = ui->PasswordlineEdit->text();
    std::string password = sha256(pass.toStdString());
    return QString::fromStdString(password);
    }
    else
        return "";
}

bool LoginForm::validateLogin(const QString &login, const QString &password) {
    return !login.isEmpty() && !password.isEmpty();
}

