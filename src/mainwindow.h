#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "ChatServer.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_userRegistrationButton_clicked();
    void on_actionCloseServer_triggered();
    //вывод всех сообщений в консольное окно
    void on_resievedMessagesButton_clicked();
    //кнопка блокировки нажата
    void on_banUserButton_clicked();
    void on_actionNew_start_server_triggered();
    void on_actionclose_programm_triggered();

    //попытка контролировать изменение размера окна
    void resizeEvent(QResizeEvent *event);
    //показ сообщений клиеттов
    void loadMessages();
    //открытие окна для выбора пользователя для блокировки
    void openUserBanDialog();
    //выполнене блокировки
    void banUser(const QString &username);
    //разблокировка
    void unBlockUser(const QString &username);

private:
    Ui::MainWindow *ui;    
    std::unique_ptr<ChatServer> chatServer;
};

#endif // MAINWINDOW_H
