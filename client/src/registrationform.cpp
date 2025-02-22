#include "registrationform.h"
#include "ui_registrationform.h"
#include "sha256.h"

#include <memory>
#include <QMessageBox>

RegistrationForm::RegistrationForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationForm)   
{
    ui->setupUi(this);   
}

RegistrationForm::~RegistrationForm() {
    delete ui;
}

void RegistrationForm::on_loginButton_clicked() {
    emit loginRequested();
}

void RegistrationForm::setServer(std::shared_ptr<ChatClient> dbPtr) {
    m_dbPtr = dbPtr;
    if(m_dbPtr->isConnected()) {
        ui->serverMessagetextEdit->append("Connected to server");
    }
    else {
        ui->serverMessagetextEdit->append("ERROR connected to server!");
    }
    //подключение сигналов от ChatClient
    connect(m_dbPtr.get(), &ChatClient::connectionError, this,
            &RegistrationForm::onConnectionError);
    connect(m_dbPtr.get(), &ChatClient::registrationResponseReceived, this,
            &RegistrationForm::onServerResponseReceived);
}


void RegistrationForm::on_buttonBox_accepted() {
    m_userName = ui->loginLineEdit->text();
    QString password = pwdEncoding(1);
    QString confirmPass = pwdEncoding(2);

    if(password == confirmPass && validateLogin(m_userName, password, confirmPass)) {
        if(m_dbPtr->isConnected()) {
        //отправка логина и пароля на сервер
        QString message = QString("REGISTER:%1:%2").arg(m_userName).arg(password);
        m_dbPtr->sendMessageRegistred(message);
        }
        else {
            QMessageBox::warning(this, "ERROR", "No connection to the server. Please try again later.");
            return;
        }
    }
    else {
        //qDebug() << "ERROR registranion!";
        QMessageBox::warning(this, "ERROR", "Incorrect login or password. Please try again.");
        ui->passwordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
        ui->loginLineEdit->setFocus();
    }
}

void RegistrationForm::onServerResponseReceived(const QString &response) {
    //qDebug() << "Response received:" << response;
    if(response.startsWith("Registration successful")) {
        QStringList parts = response.split(":");
        int number;
        if(parts.size() == 3) {
            qDebug() << parts[1];
            bool convert;
            number = parts[2].toInt(&convert);
            if(convert) {              
                qDebug() << number;
            }
            else
                qDebug() << "ERROR userId not integer!";
        }        
        // Успешная регистрация m_dbPtr,
        emit accepted(parts[1], number);
    }
    else {
        // Ошибка при регистрации
        QMessageBox::warning(this, "ERROR", "Not Registration. Username might be taken. Please try again.");
        ui->loginLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
        ui->loginLineEdit->setFocus();
    }
}

void RegistrationForm::onConnectionError(const QString &errorMessage) {
    QMessageBox::warning(this, "Connection Error", errorMessage);
    // очистка полей ввода
    ui->loginLineEdit->clear();
    ui->passwordLineEdit->clear();
    ui->confirmPasswordLineEdit->clear();
    ui->loginLineEdit->setFocus();
}

QString RegistrationForm::pwdEncoding(short n) {
    SHA256 sha256;
    QString pass;
    if(n == 1)
        pass = ui->passwordLineEdit->text();
    else
        pass = ui->confirmPasswordLineEdit->text();

    std::string password = sha256(pass.toStdString());
    return QString::fromStdString(password);
}

bool RegistrationForm::validateLogin(const QString &login, const QString &password,
                                     const QString &confirmPas) {
    return !login.isEmpty()  && !password.isEmpty() && !confirmPas.isEmpty();
}

void RegistrationForm::on_buttonBox_rejected() {
    emit rejected();
}

