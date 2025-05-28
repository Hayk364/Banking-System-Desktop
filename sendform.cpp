#include "sendform.h"
#include "ui_sendform.h"
#include "backend.h"
#include <variant>
#include <string>

SendForm::SendForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SendForm)
{
    ui->setupUi(this);
}

SendForm::~SendForm()
{
    delete ui;
}

void SendForm::on_send_pushButton_clicked()
{
    std::string token = ui->token_textField->text().toStdString();
    float count = ui->count_textField->text().toFloat();

    auto result = Backend::Send(token,count);

    if(std::holds_alternative<bool>(result)){
        ui->error_label->setText("True");
        std::this_thread::sleep_for(std::chrono::microseconds(3000));
        this->close();
    }else{
        std::string error = std::get<std::string>(result);
        ui->error_label->setText(QString::fromStdString(error));
    }

}

