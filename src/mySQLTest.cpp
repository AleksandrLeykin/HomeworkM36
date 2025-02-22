 #include "mySQLTest.h"
#include <QDebug>

mySQLTest::mySQLTest() {
    if(mySQLDB()){
        qDebug() << "connection DB is ok!";
    }
    else {
        qDebug() << "ERROR!!! not connection DB";
    }
}

mySQLTest::~mySQLTest() {
    mysql_close(&mysql);
    qDebug() << "mysql exit!";
}

QString mySQLTest::getUser() {
    int i = 0;
    std::string result = "";
    //запрос
    std::string selectUser = "SELECT name from users";
    mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());
    if ((res = mysql_store_result(&mysql))) {
        while ((row = mysql_fetch_row(res))) {
            for (i = 0; i < mysql_num_fields(res); i++) {
                result += row[i];
            }
            result += "\n";
        }
    }
    else {
        qDebug() << "ERROR request";
    }
    if(result == "")
        qDebug() << "not client registration!";
    return QString::fromStdString(result);
}

QString mySQLTest::viewMessages() {
    int i = 0;
    std::string result = "";
    //запрос
    //std::string selectMessage = "select * from messages";
    std::string selectMessage = "select CONCAT(user1_id, ' ' , user2_id, ' ' , data_create, ' ' , messages) from messages";
    mysql_real_query(&mysql, selectMessage.c_str(), selectMessage.length());
    if ((res = mysql_store_result(&mysql))) {
        while ((row = mysql_fetch_row(res))) {
            for (i = 0; i < mysql_num_fields(res); ++i) {
                result += row[i];
            }
            result += "\n";
        }
        mysql_free_result(res);
    }
    else {
        qDebug() << "ERROR request";
    }
    if(result == "")
        qDebug() << "messages Empty!";
    return QString::fromStdString(result);
}

QString mySQLTest::userViewMessages(int id) {
    int i = 0;
    std::string result = "";
    //запрос
    std::string selecMessages = "select CONCAT(u.name, ' ' ,  m.data_create, ' ' , m.messages) "
                                "from users as u join messages as m "
                                "on u.id = m.user2_id "
                                "where m.user1_id = '" + std::to_string(id) + "'";
    if(mysql_real_query(&mysql, selecMessages.c_str(), selecMessages.length())) {
        qDebug() << "Error MySql # " << mysql_error(&mysql);
        return QString();
    }

    if((res = mysql_store_result(&mysql))) {
        while((row = mysql_fetch_row(res))) {
            for(i = 0; i < mysql_num_fields(res); ++i) {
                result += row[i];
            }
            result += "\n";
        }
        mysql_free_result(res);
    }
    else {
        qDebug() << "ERROR request";
    }
    if(result == "")
        qDebug() << "messages Empty!";

    return QString::fromStdString(result);
}

bool mySQLTest::addingUser(const QString &name, const QString &pas) {
    int i =0;
    //подготовка запроса
    std::string record_user = "INSERT INTO users(id, name, password)"
                              " VALUES(default, '" + name.toStdString() + "',  '" + pas.toStdString() + "')";
    //запрос
    std::string selectUser = "SELECT name, password FROM users";
    mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());

    //строка с новым именем
    std::string newName = name.toStdString() + pas.toStdString();
    //проверка имени в таблице через цикл
    if ((res = mysql_store_result(&mysql))) {
        while ((row = mysql_fetch_row(res))) {
            std::string currentName = "";
            for (i = 0; i < mysql_num_fields(res); ++i) {
                currentName += row[i];
            }           
            if (newName == currentName) {
                mysql_close(&mysql);
                return false;
            }
        }
    }
    else
        qDebug() << "Error MySql # " << mysql_error(&mysql);
    //если имя не занято записываем его
    mysql_real_query(&mysql, record_user.c_str(), record_user.length());
    mysql_close(&mysql);
    return true;
}

bool mySQLTest::userLogin(const QString &name, const QString &pas) {
    int i = 0;
    //подготовка запроса
    std::string selectUser = "SELECT name, password FROM users";
    mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());
    //строка с именем
    std::string newName = name.toStdString() + pas.toStdString();
    //проверка имени в таблице через цикл
    if ((res = mysql_store_result(&mysql))) {
        while ((row = mysql_fetch_row(res))) {
            std::string currentName = "";
            for (i = 0; i < mysql_num_fields(res); ++i) {
                currentName += row[i];
            }
            if (newName == currentName) {
                if(!isBaned(name, pas)){
                    return false;
                }

                mysql_close(&mysql);
                return true;
            }
        }
    }
    else
        qDebug() << "Error MySql # " << mysql_error(&mysql);
    return false;
}

bool mySQLTest::isBaned(const QString name, const QString pas) {
    std::string selectBan = "SELECT is_ban FROM users WHERE name = '"
                            + name.toStdString() + "' AND password = '" + pas.toStdString() + "'";

    if(mysql_real_query(&mysql, selectBan.c_str(), selectBan.length())) {
        qDebug() << "Error MySql # " << mysql_error(&mysql);
        return false;
    }

    MYSQL_RES *result = mysql_store_result(&mysql);
    if (result == nullptr) {
        qDebug() << "Result storage error: " << mysql_error(&mysql);
        return false;
    }

    MYSQL_ROW row1;
    if((row1 = mysql_fetch_row(result)) != nullptr) {
        int isBan = std::stoi(row1[0]);
        if(isBan) {
            qDebug() << "Banned client login attempt - " << name;
            return false;
        }
    }
    return true;
}

 bool mySQLTest::writingMessage(const QString& name1, const QString& name2,
                               const QString& strMes) {
     std::string selectMessage = "INSERT INTO messages(id, user1_id, user2_id, data_create, messages)"
                                 " VALUES(default, "
                                 "(select id from users where name = '" + name1.toStdString() + "'), "
                                 "(select id from users where name = '" + name2.toStdString() + "'), "
                                 "(select sysdate()), "
                                 "'" + strMes.toStdString() + "')";
     if (mysql_real_query(&mysql, selectMessage.c_str(), selectMessage.length()) == 0) {
         return true;
     }
     return false;
 }

 bool mySQLTest::writingMessageAll(const QString &name, const QString &strMess)  {

    QStringList usersName = getUser().split('\n');

    if(!usersName.isEmpty()) {
        for(auto name2 : usersName) {
            if(!name2.isEmpty())
                writingMessage(name2, name, strMess);
        }
        return true;
    }
    return false;
 }




 QString mySQLTest::loadRegistretedUsers() {
    int i = 0;
    std::string result = "";
    //запрос
    std::string selectUser = "select name password from users";
    mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());
    if ((res = mysql_store_result(&mysql))) {
        while ((row = mysql_fetch_row(res))) {
            for (i = 0; i < mysql_num_fields(res); i++) {
                result += row[i];
            }
            result += "/n";
        }
    }
    else {
        qDebug() << "ERROR request";
    }
    if(result == "")
        qDebug() << "not client registration!";
    return QString::fromStdString(result);
 }

 QString mySQLTest::userRegistrftion() {
    int i = 0;
    std::string result = "USERS:";
    //запрос
    std::string selectUser = "select CONCAT(id, ' ', name) from users";
    mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());
    if ((res = mysql_store_result(&mysql))) {
        while ((row = mysql_fetch_row(res))) {
            for (i = 0; i < mysql_num_fields(res); i++) {
                result += row[i];
            }
            result += ":";
        }
    }
    else {
        qDebug() << "ERROR request";
    }
    if(result == "")
        qDebug() << "not client registration!";
    return QString::fromStdString(result);
 }

 QString mySQLTest::userRegistrftionServer()  {
     int i = 0;
     std::string result = "";
     //запрос
     std::string selectUser = "SELECT CONCAT(id, ' ', name) from users";
     mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());
     if ((res = mysql_store_result(&mysql))) {
         while ((row = mysql_fetch_row(res))) {
             for (i = 0; i < mysql_num_fields(res); i++) {
                 result += row[i];
             }
             result += "\n";
         }
     }
     else {
         qDebug() << "ERROR request";
     }
     if(result == "")
         qDebug() << "not client registration!";
     return QString::fromStdString(result);
 }

 QString mySQLTest::idName(const QString m_name, const QString pass)
 {
     QString userId;

    std::string selectIdName = "SELECT id FROM users WHERE name = '" + m_name.toStdString() + "' AND password = '" + pass.toStdString() + "'";
    if (mysql_real_query(&mysql, selectIdName.c_str(), selectIdName.length())) {
        // Если произошла ошибка выполнения запроса
        qDebug() << "Query failed: " << mysql_error(&mysql);
        return QString();
    }
    if((res = mysql_store_result(&mysql))) {
        // Получаем количество строк результата
        int numRows = mysql_num_rows(res);
        if (numRows > 0) {
            // Извлекаем результат
            row = mysql_fetch_row(res);
            if (row && row[0]) { // Проверка на NULL
                userId = QString::fromStdString(row[0]); // Преобразуем результат в QString
            }
        }
        // Освобождаем ресурс
        mysql_free_result(res);
    }
    else {
        // Если не удалось получить результат
        qDebug() << "No result returned: " << mysql_error(&mysql);
    }
    return userId;
 }

 void mySQLTest::banedUser(const QString &username) {
    //запрос
    std::string banUser = "UPDATE users SET is_ban = 1 WHERE name = '" + username.toStdString() + "'";
    if(mysql_real_query(&mysql, banUser.c_str(), banUser.length()) != 0) {
        qDebug() << "Error the client is not banned!";
    }
    else
        qDebug() << "The client is banned! name = " + username;
 }

 void mySQLTest::unblockUser(const QString username) {
     //запрос
     std::string banUser = "UPDATE users SET is_ban = 0 WHERE name = '" + username.toStdString() + "'";
     if(mysql_real_query(&mysql, banUser.c_str(), banUser.length()) != 0) {
         qDebug() << "Error the client is banned!";
     }
     else
         qDebug() << "The client is unlocked! name = " + username;

 }

bool mySQLTest::mySQLDB()
{
    if(connectDB()) {
        //Сооздание базы данных
        QString createDB = "CREATE DATABASE if not EXISTS chatQTdb";
        QString useDB = "use chatQTdb";

        if(mysql_real_query(&mysql, createDB.toUtf8().constData(),
                            createDB.toUtf8().length()) != 0) {
            qDebug() << "Error DATABASE mySQL request not sent!";
            return false;
        }
        //qDebug() << "DATABASE mySQL request!";

        //подключаемся к базе
        if (mysql_real_query(&mysql, useDB.toUtf8().constData(),
                             useDB.toUtf8().length()) != 0)
        {
            qDebug() << "Error use DATABASE mySQL!";
            return false;
        }
        //qDebug() << "use DATABASE mySQL!";

        //Создание таблиц
        std::string createTable = "CREATE TABLE IF NOT EXISTS users("
                                  "id SERIAL PRIMARY KEY AUTO_INCREMENT, "
                                  "name VARCHAR(255) NOT NULL UNIQUE, "
                                 // "surname VARCHAR(255) NOT NULL, "
                                 // "email VARCHAR(255) NOT NULL, "
                                  "password VARCHAR(100) NOT NULL, "
                                  "is_ban TINYINT(1) NOT NULL DEFAULT 0)";
        if (mysql_real_query(&mysql, createTable.c_str(), createTable.length()) != 0) {
            qDebug() << "Error table mySQL request not sent! ";
            return false;
        }
        //qDebug() << "table user create!";

        //_____________________________________________________________________________________________
        // Вставка пользователя admin
        std::string insertAdmin = "INSERT INTO users (name, password, is_ban) "
                                  "VALUES ('admin', '" + admin_password + "', 0) "
                                  "ON DUPLICATE KEY UPDATE id=id";
        if (mysql_real_query(&mysql, insertAdmin.c_str(), insertAdmin.length()) != 0) {
            qDebug() << "Error inserting admin user: " << mysql_error(&mysql);
            return false;
        }
        //_____________________________________________________________________________________________

        //user1_id - получатель, user2_id - отправитель
        std::string createTableMessage = "CREATE TABLE IF NOT EXISTS messages("
                                         "id SERIAL PRIMARY KEY AUTO_INCREMENT, "
                                         "user1_id integer NOT NULL REFERENCES users(id), "
                                         "user2_id integer NOT NULL REFERENCES users(id), "
                                         "data_create DATETIME NOT NULL, "
                                         "messages TEXT NOT NULL)";
        if (mysql_real_query(&mysql, createTableMessage.c_str(), createTableMessage.length()) != 0) {
            qDebug() << "Error table mySQL request not sent! ";
            return false;
        }
        //qDebug() << "table messages create!";

        return true;
    }
    else
        return false;
}

bool mySQLTest::connectDB() {
    if (!mysql_init(&mysql)) {
        qDebug() << "Error: can`t create MySQL - description";
        return false;
    }
    // Подключаемся к серверу
    if (!mysql_real_connect(&mysql, "localhost", "root", "root", nullptr, 0, nullptr, 0)) {
        // Если нет возможности установить соединение с БД выводим сообщение об ошибке
         qDebug() << "Error: can`t connect to database " << mysql_error(&mysql);
        return false;
    }
    mysql_set_character_set(&mysql, "utf8");
    return true;
}
