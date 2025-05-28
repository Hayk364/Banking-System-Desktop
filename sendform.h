#ifndef SENDFORM_H
#define SENDFORM_H

#include <QWidget>

namespace Ui {
class SendForm;
}

class SendForm : public QWidget
{
    Q_OBJECT

public:
    explicit SendForm(QWidget *parent = nullptr);
    ~SendForm();

private slots:
    void on_send_pushButton_clicked();

private:
    Ui::SendForm *ui;
};

#endif // SENDFORM_H
