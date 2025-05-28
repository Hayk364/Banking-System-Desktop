#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "backend.h"
#include <thread>
#include <future>
#include "homewindow.h"
#include "createwallet.h"
#include <QSettings>
#include "backend.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool check = false;

void MainWindow::on_radioButton_toggled(bool checked)
{
    check = checked;
    if(checked){
        ui->label->setText("Registration");
        ui->pushButton->setText("Registartion");
    }else{
        ui->label->setText("Login");
        ui->pushButton->setText("Login");
    }
}


void MainWindow::on_pushButton_clicked()
{
    QSettings settings("LuX","Bank");
    if(check){
        std::string username = ui->UsernameTextLine->text().toStdString();
        std::string password = ui->PasswordTextLine->text().toStdString();

        std::future<TokenBody> res = std::async(std::launch::async,Backend::Register,username,password);

        try{
            TokenBody result = res.get();
            if(result.success){
                settings.setValue("auth/token",QString::fromStdString(result.token));
                settings.setValue("username",QString::fromStdString(username));
                settings.setValue("password",QString::fromStdString(password));
                settings.setValue("id",result.id);

                CreateWallet* createwallet = new CreateWallet();
                createwallet->show();

                this->close();
            }else{
                ui->label->setText("Register Failed");
            }
        }catch(const std::exception& e){
            std::cout << e.what() << std::endl;
        }
    }else{
        std::string username = ui->UsernameTextLine->text().toStdString();
        std::string password = ui->PasswordTextLine->text().toStdString();

        std::future<TokenBody> res = std::async(std::launch::async,Backend::Login,username,password);

        try{
            TokenBody result = res.get();
            if(result.success){
                settings.setValue("auth/token",QString::fromStdString(result.token));
                settings.setValue("username",QString::fromStdString(username));
                settings.setValue("password",QString::fromStdString(password));
                settings.setValue("id",result.id);

                std::string name = Backend::GetNameSurname(result.id);
                settings.setValue("nameAndSurname",QString::fromStdString(name));

                HomeWindow* home = new HomeWindow();
                home->show();

                this->close();
            }else{
                ui->label->setText("Login Failed");
            }
        }catch(const std::exception& e){
            std::cout << e.what() << std::endl;
        }
    }
}

