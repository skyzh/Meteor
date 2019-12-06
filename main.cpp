#include "mainwindow.h"

#include "db.h"

#include <QApplication>
#include <QPointer>

QPointer<MainWindow> run_application(int argc, char *argv[]) {
    DB::init();

    QPointer<MainWindow> w = QPointer<MainWindow>(new MainWindow);
    w->show();

    return w;
}
