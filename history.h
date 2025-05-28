#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QStringListModel>

namespace Ui {
class History;
}

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    ~History();

    void setTransactions(const QStringList &transactions);

private:
    Ui::History *ui;
    QStringListModel *userModel;
    QStringListModel *friendModel;
};

#endif // HISTORY_H
