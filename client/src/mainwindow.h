#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::shared_ptr<ChatClient> chatPtr, QString userName, int userId, QWidget *parent = nullptr);
    ~MainWindow();
    //счетчик клиентов
    static int InstanceCountClient;
    //создание подключенного нового клиента
    static MainWindow* createClient();

private slots:
    // запрос списка пользователей request list of users
    void openUserNameregistedDialog();
    //запрос списка приватных сообщений Request a list of private messages
    void openPrivateMessageList();

    //обработка сообщений от ChatClient(через него от сервера) открытие окна для выбора users
    void handleMessage(const QString &message);

    //обработка приватного сообщения processing private message
    void sendMessageToUser(const QString &userName, const QString &message);
    //open dialog открытие нового окна
    QString openDialogListUser(QStringList list);
    //Обработка отправки сообщения для всех
    void handleSendMessage();
    //this time
    QString logCurrentDateTime();

    //реaкция на вкладки меню
    void on_actionConnecting_a_new_client_triggered();
    void on_actionClose_client_triggered();

    void on_sendMessageButton_clicked();
    void on_privateMessageButton_clicked();    
    void on_messageLineEdit_returnPressed();

    void on_messageForMeButton_clicked();

private:
    Ui::MainWindow *ui;

    // Создание экземпляра ChatClient
    std::shared_ptr<ChatClient> m_chatPtr;
    int m_userId;
    QString m_userName;

    //хранение полученных сообщений
    QStringList m_users;
};

#endif // MAINWINDOW_H
