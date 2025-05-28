#ifndef CREATEWALLET_H
#define CREATEWALLET_H

#include <QMainWindow>

namespace Ui {
class CreateWallet;
}

class CreateWallet : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreateWallet(QWidget *parent = nullptr);
    ~CreateWallet();

private slots:
    void on_sendWalletDataButton_clicked();

private:
    Ui::CreateWallet *ui;
};

#endif // CREATEWALLET_H
