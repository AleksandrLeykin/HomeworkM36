#ifndef BANEDUSERFORM_H
#define BANEDUSERFORM_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

namespace Ui {
class BanedUserForm;
}

class BanedUserForm : public QWidget
{
    Q_OBJECT

public:
    explicit BanedUserForm(QWidget *parent = nullptr);
    ~BanedUserForm();

    void setUserList(const QStringList &users);

signals:
    void userBanned(const QString &username);

private slots:
    void on_BanButton_clicked();

private:
    Ui::BanedUserForm *ui;
    QListWidget *userListWidget;
    QPushButton *banButton;
};

#endif // BANEDUSERFORM_H
