#ifndef MYSQLTEST_H
#define MYSQLTEST_H

#include <mysql.h>
#include <string>
#include <QString>

class mySQLTest
{
public:
    mySQLTest();
    ~mySQLTest();

    //просмотр пользователей
    QString getUser();   
    //добавление пользователя set user
    bool addingUser(const QString& name, const QString& pas);
    // вход пользователя enter user
    bool userLogin(const QString &name, const QString &pas);

    //view messages просмотр сообщений
    QString viewMessages();
    //view messages user просмотр сообщений пользователя
    QString userViewMessages(int id);
    //writing Message запись сообщения
    bool writingMessage(const QString& name1, const QString& name2, const QString& strMes);
    //запись сообщения всем
    bool writingMessageAll(const QString& name, const QString& strMess);

    //список зарегистрированных пользователей
    QString loadRegistretedUsers();
    //список пользователей для передачи клиентам
    QString userRegistrftion();
    //список пользователей для передачи server
    QString userRegistrftionServer();

    //id по имени
    QString idName(const QString m_name, const QString pass);

    //бан пользователя
    void banedUser(const QString& username);
    //снятие блокировки
    void unblockUser(const QString username);
    bool isBaned(const QString m_name, const QString pas);

private:
    //дескриптор
    MYSQL mysql;
    //ссылка на структуру MYSQL_RES результат
    MYSQL_RES* res;
    //Представляет одну строку из результата, содержащего строки.
    MYSQL_ROW row;
    //создание таблиц
    bool mySQLDB();
    //подключение к БД
    bool connectDB();
    std::string admin_password = "12345";
};

#endif // MYSQLTEST_H
