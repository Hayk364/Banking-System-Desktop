#include "createwallet.h"
#include "ui_createwallet.h"
#include "backend.h"
#include <thread>
#include <future>
#include <QSettings>
#include "homewindow.h"


CreateWallet::CreateWallet(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreateWallet)
{
    ui->setupUi(this);
}

CreateWallet::~CreateWallet()
{
    delete ui;
}

void CreateWallet::on_sendWalletDataButton_clicked()
{
    QSettings settings("LuX","Bank");
    std::string nameAndsurname = ui->NameSurnameTextField->text().toStdString();
    std::string email = ui->EmailTextField->text().toStdString();
    settings.setValue("nameAndSurname",QString::fromStdString(nameAndsurname));
    std::cout << settings.value("nameAndSurname").toString().toStdString() << std::endl;

    std::future<bool> result = std::async(std::launch::async,Backend::Createwallet,nameAndsurname,email);

    try{
        bool res = result.get();
        if(res) {
            HomeWindow* home = new HomeWindow();
            home->show();

            this->close();
        }else{
            ui->label->setText("Error");
        }
    }catch(const std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

