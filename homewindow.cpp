#include "homewindow.h"
#include "ui_homewindow.h"
#include <QSettings>
#include <QClipboard>
#include <QString>
#include "mainwindow.h"
#include "backend.h"
#include "sendform.h"
#include "history.h"
#include <thread>

HomeWindow::HomeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    QSettings settings("LuX","Bank");
    MainBody data = Backend::GetMainBody();
    ui->token_label->setText(QString::fromStdString(data.token));
    ui->balance_label->setText(QString::number(data.balance));
    ui->namesurname_label->setText(QString::fromStdString(data.NameAndSurname));
    settings.setValue("balance",data.balance);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &HomeWindow::updateBalance);
    updateTimer->start(3000);

}

HomeWindow::~HomeWindow()
{
    delete ui;
}

void HomeWindow::updateBalance()
{
    int id = QSettings("LuX", "Bank").value("id").toInt();

    std::thread t([this, id]() {
        float balance = Backend::UpdateBalance(id);

        QMetaObject::invokeMethod(this, [this, balance]() {
            QSettings("LuX","Bank").setValue("balance",balance);
            ui->balance_label->setText(QString::number(balance));
        }, Qt::QueuedConnection);
    });
    t.detach();
}

void HomeWindow::on_logoutPushButton_clicked()
{
    QSettings settings("LuX","Bank");
    settings.remove("auth/token");
    settings.remove("username");
    settings.remove("password");
    settings.remove("nameAndSurname");
    settings.remove("id");
    MainWindow* m = new MainWindow();
    this->close();
    m->show();
}


void HomeWindow::on_label_3_linkActivated(const QString &link){}


void HomeWindow::on_tokenCopypushButton_clicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->token_label->text());
}



void HomeWindow::on_sendpushButton_clicked()
{
    SendForm* sendform = new SendForm();
    sendform->show();
}


void HomeWindow::on_balance_label_linkActivated(const QString &link) {}


void HomeWindow::on_historypushButton_clicked()
{
    History* history = new History();
    history->show();
}

