#ifndef USERBANDIALOG_H
#define USERBANDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace Ui {
class UserBanDialog;
}

class UserBanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserBanDialog(QWidget *parent = nullptr);

    void setUserList(const QStringList &users);

signals:
    void userBanned(const QString &username);
    void unblock(const QString &username);

private slots:
    void on_BanButton_clicked();
    void on_unblockButton_clicked();
    void my_styles();

private:
    QListWidget *userListWidget;
    QPushButton *banButton;
    QPushButton *unblockButton;
};

#endif // USERBANDIALOG_H
