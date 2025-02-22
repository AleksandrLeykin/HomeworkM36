#include "startscreeen.h"
#include "ui_startscreeen.h"


StartScreeen::StartScreeen(std::shared_ptr<ChatClient> dbPtr, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartScreeen)
{
    ui->setupUi(this);
    //заголовок
    this->setWindowTitle("Enter chat");

    if(dbPtr)
        m_dbPtr = dbPtr;
    else
        m_dbPtr = std::make_shared<ChatClient>("127.0.0.1", 7777);

    //добавить проверку подключения к серверу
    ui->loginWidget->setServer(m_dbPtr);
    ui->registrWidget->setServer(m_dbPtr);

    if (ui->loginWidget && ui->registrWidget) {
    connect(ui->loginWidget, &LoginForm::registrRequested, this, &StartScreeen::setRegistrationForm);   
    connect(ui->loginWidget, &LoginForm::accepted, this, &StartScreeen::onLoggedIn);
    connect(ui->loginWidget, &LoginForm::rejected, this, &StartScreeen::onRejectRequested);

    connect(ui->registrWidget, &RegistrationForm::loginRequested, this, &StartScreeen::setLoginForm);   
    connect(ui->registrWidget, &RegistrationForm::accepted, this, &StartScreeen::onLoggedIn);
    connect(ui->registrWidget, &RegistrationForm::rejected, this, &StartScreeen::onRejectRequested);
    } else {
        qDebug() << "One of the widgets is not initialized!";
    }
}

StartScreeen::~StartScreeen()
{
    //qDebug() << "StartScreeen: destructor!!!!";
    delete ui;
}

void StartScreeen::setLoginForm()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void StartScreeen::setRegistrationForm()
{
    ui->stackedWidget->setCurrentIndex(1);
}

//std::shared_ptr<ChatClient> dbPtr,
void StartScreeen::onLoggedIn(QString userName, uint userId) {
    m_userName = userName;
    m_userId = userId;
    //m_dbPtr = dbPtr;
    //ждем подключения к серверу
    accept();   
}

int StartScreeen::userId() const {
    //qDebug() << "StartScreeen: User id in successfully: " << m_userId;
    return m_userId;
}

QString StartScreeen::userName() const {
    //qDebug() << "StartScreeen: User Name in successfully: " << m_userName;
    return m_userName;
}

std::shared_ptr<ChatClient> StartScreeen::getDbptr() const {
    //qDebug() << "StartScreeen: ChatPtr in successfully:";
    return m_dbPtr;
}

//пробую закрыть форму временно!!
void StartScreeen::m_close() {
    //qDebug() << "StartScreeen: close!";
    this->close();
}

void StartScreeen::onRejectRequested() {
    //qDebug() << "StartScreeen: User rejected the login/registration.";
    reject();
}
