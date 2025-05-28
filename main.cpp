#include "mainwindow.h"
#include "homewindow.h"

#include <QApplication>
#include <QSettings>
#include <QString>
#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings settings("LuX","Bank");
    QString token = settings.value("auth/token","").toString();

    std::unique_ptr<QMainWindow> window;

    if(!token.isEmpty()){
        window = std::make_unique<HomeWindow>();
    }else{
        window = std::make_unique<MainWindow>();
    }
    window->show();
    return a.exec();
}
