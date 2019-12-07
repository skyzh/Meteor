#include "mainwindow.h"

#include "db.h"
#include "ConfigManager.h"

#include <QApplication>
#include <QPointer>

QPointer<MainWindow> run_application(int argc, char *argv[]) {
    DB::init();
    ConfigManager::instance();

    QPointer<MainWindow> w = QPointer<MainWindow>(new MainWindow);
    w->show();

    return w;
}
