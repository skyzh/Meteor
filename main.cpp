#include "mainwindow.h"

#include <QApplication>
#include <QSqlDatabase>
#include "db.h"

int main(int argc, char *argv[])
{
    DB::init();
    
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
