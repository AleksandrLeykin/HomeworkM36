#include "baneduserform.h"
//#include "ui_baneduserform.h"

BanedUserForm::BanedUserForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BanedUserForm)
{
    ui->setupUi(this);
}

BanedUserForm::~BanedUserForm()
{
    delete ui;
}

void BanedUserForm::on_BanButton_clicked()
{

}

