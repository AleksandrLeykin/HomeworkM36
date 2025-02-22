#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QResizeEvent>

#include "QPlainTextBrowserLogger.h"
#include "mySQLTest.h"
#include "userbandialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{    
    chatServer = std::make_unique<ChatServer>(this);
    ui->setupUi(this);

    this->setStyleSheet("QMainWindow {background-color: orange;}");
    // Устанавливаем заголовок окна
    setWindowTitle("ChatServer");
    // Установка минимального размера окна
    setMinimumSize(1200, 400);

    // Подключаем сигнал нажатия кнопки к слоту
    connect(ui->userRegistrationButton, &QPushButton::clicked, this, &MainWindow::on_userRegistrationButton_clicked);

    //__________________________________________________________________________
    // Перенаправляем соoбщения консоли на форму
    QPlainTextEditLogger *logger = new QPlainTextEditLogger(ui->consoleBrowser);
    qApp->setProperty("logger", QVariant::fromValue<void*>(logger));
    //__________________________________________________________________________

    chatServer->startServer(7777);
}

MainWindow::~MainWindow() {
    if (chatServer != nullptr)
        chatServer->closeServer();
    delete ui;    
}

void MainWindow::on_userRegistrationButton_clicked() {
    // подключаемся к БД
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    // Вывод текста в QTextBrowser
    ui->listOfClientsBrowser->setText(mdb->userRegistrftionServer());
}

void MainWindow::on_actionCloseServer_triggered() {
    chatServer->closeServer();
    chatServer.reset();
    if(chatServer == nullptr) {
        qDebug() << "Server close!";
    }
}

void MainWindow::on_actionNew_start_server_triggered() {   
    chatServer.reset();
    chatServer = std::make_unique<ChatServer>(this);
    chatServer->startServer(7777);
}

void MainWindow::loadMessages() {
    // подключаемся к БД
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    ui->textEdit->append(mdb->viewMessages());
}

void MainWindow::openUserBanDialog() {
    UserBanDialog dialog(this);

    // Получуние списока пользователей (из базы данных)
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    QStringList users = mdb->getUser().split('\n');
    dialog.setUserList(users);
    //здесь реализуем сигнал на бан клиента
    connect(&dialog, &UserBanDialog::userBanned, this, &MainWindow::banUser);
    //реализация сигнала на снятие блокировки
    connect(&dialog, &UserBanDialog::unblock, this, &MainWindow::unBlockUser);
    // Открыть диалог
    dialog.exec();
}

void MainWindow::banUser(const QString &username) {
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    mdb->banedUser(username);
}

void MainWindow::on_resievedMessagesButton_clicked() {
    ui->textEdit->clear();
    loadMessages();
}

void MainWindow::on_banUserButton_clicked() {
    openUserBanDialog();
}

void MainWindow::unBlockUser(const QString &username) {
    std::unique_ptr<mySQLTest> mdb{ new mySQLTest };
    mdb->unblockUser(username);
}

void MainWindow::on_actionclose_programm_triggered() {
    if (chatServer != nullptr)
        chatServer->closeServer();
    this->close();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    // Получаем ширину родительского окна
    int totalWidth = event->size().width();

    ui->consoleBrowser->setFixedWidth(totalWidth * 0.4);
    ui->textEdit->setFixedWidth(totalWidth * 0.4);
    ui->listOfClientsBrowser->setFixedWidth(totalWidth * 0.2);
}

