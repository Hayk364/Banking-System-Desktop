#include "history.h"
#include "ui_history.h"
#include "backend.h"
#include <QSettings>

History::History(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::History),
    userModel(new QStringListModel(this)),
    friendModel(new QStringListModel(this))
{
    ui->setupUi(this);
    QSettings settings("LuX","Bank");


    QStringList friendSampleTransactions = Backend::GetHistoryList(settings.value("id").toInt(),false);
    friendModel->setStringList(friendSampleTransactions);
    ui->friendSendList->setModel(friendModel);
    qDebug() << "Friend history:" << friendSampleTransactions;


    QStringList userSampleTransactions = Backend::GetHistoryList(settings.value("id").toInt(),true);
    userModel->setStringList(userSampleTransactions);
    ui->userSendList->setModel(userModel);
    qDebug() << "User history:" << userSampleTransactions;
}

History::~History()
{
    delete ui;
}
