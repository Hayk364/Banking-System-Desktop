#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H
#include <QTimer>
#include <QMainWindow>
#include <iostream>
#include <string>

struct UserDataBody{
    float balance;
    std::string token;
};

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent = nullptr);
    ~HomeWindow();

private slots:
    void on_logoutPushButton_clicked();

    void on_label_3_linkActivated(const QString &link);

    void on_tokenCopypushButton_clicked();

    void on_sendpushButton_clicked();

    void updateBalance();

    void on_balance_label_linkActivated(const QString &link);

    void on_historypushButton_clicked();

private:
    Ui::HomeWindow *ui;
    QTimer *updateTimer;
};

#endif // HOMEWINDOW_H
