#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User(int id, const QString &name)
        : m_id(id), m_userName(name) {}

    int getId() const { return m_id; }
    QString getName() const { return m_userName; }

private:
    int m_id;
    QString m_userName;
};

#endif // USER_H
