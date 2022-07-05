#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>

#include "SQL.h"

SQL SQLL;
list<device> device_list;

int level_log = 2;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // qSetMessagePattern("[%{time yyyy-MM-dd h:mm:ss}]");
    Logger::initLog();
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

    // w.setWindowFlags(Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint);

    //    w.show();
    return a.exec();
}
