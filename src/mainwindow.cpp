#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QSizePolicy>
#include <QGraphicsLayout>
#include <QtAlgorithms>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), scheduler(this) {
    ui->setupUi(this);
    setup_status_bar();
    ui->comboStation->addItem("All", "All");
    {
        ui->comboLine->addItem("All", "All");
        ui->comboLine->addItem("A - Line 4", "A");
        ui->comboLine->addItem("B - Line 1", "B");
        ui->comboLine->addItem("C - Line 2", "C");
    }
    {
        ui->comboBoxFlow->addItem("A - Line 4 (Puyan - Pengbu)", "A");
        ui->comboBoxFlow->addItem("B - Line 1 (Xianghu - Linping)", "B1");
        ui->comboBoxFlow->addItem("B - Line 1 (Xianghu - Xiasha Jiangbin)", "B2");
        ui->comboBoxFlow->addItem("C - Line 2 (Liangzhu - Chaoyang)", "C");
    }
    {
        ui->comboBoxSmartWeekday->addItem("Monday", QDate(2019, 1, 7));
        ui->comboBoxSmartWeekday->addItem("Tuesday", QDate(2019, 1, 8));
        ui->comboBoxSmartWeekday->addItem("Wednesday", QDate(2019, 1, 9));
        ui->comboBoxSmartWeekday->addItem("Thursday", QDate(2019, 1, 10));
        ui->comboBoxSmartWeekday->addItem("Friday", QDate(2019, 1, 11));
        ui->comboBoxSmartWeekday->addItem("Saturday", QDate(2019, 1, 12));
        ui->comboBoxSmartWeekday->addItem("Sunday", QDate(2019, 1, 13));
    }

    delayed_chart_update = new QTimer(this);

    connect(delayed_chart_update, &QTimer::timeout, this, [=] {
        delayed_chart_update->stop();
        update_passenger_chart();
    });

    metroWidgetRoute = new MetroWidget(&metroRoutePainter, this);
    metroWidgetFlow = new MetroWidget(&metroFlowPainter, this);
    ui->layoutRoute->addWidget(metroWidgetRoute, 0, 1);
    ui->layoutFlow->addWidget(metroWidgetFlow);
    lst_flow_block = -1;

    connect(&scheduler, &TaskScheduler::progress, this, &MainWindow::progress);
    connect(&scheduler, &TaskScheduler::message, this, &MainWindow::message);

    schedulerProgress->setValue(0);
}

MainWindow::~MainWindow() {
    scheduler.quit();
    scheduler.wait();
    delete ui;
}

void MainWindow::progress(qreal percent) {
    if (percent == 0.0) {
        schedulerProgress->setRange(0, 0);
    } else {
        schedulerProgress->setRange(0, 100);
        schedulerProgress->setValue(int(percent * 100));
    }
}

void MainWindow::message(QString msg) {
    schedulerMessage->setText(msg);
}

void MainWindow::setup_status_bar() {
    schedulerMessage = new QLabel(this);
    schedulerProgress = new QProgressBar(this);
    schedulerProgress->setMinimum(0);
    schedulerProgress->setMaximum(100);
    ui->statusbar->addPermanentWidget(schedulerProgress, 1);
    ui->statusbar->addPermanentWidget(schedulerMessage, 0);
}

void MainWindow::on_actionPassenger_Traffic_triggered() {
    ui->tabWidget->setCurrentWidget(ui->tab_passenger_traffic);
}

void MainWindow::on_actionRoute_Planning_triggered() {
    ui->tabWidget->setCurrentWidget(ui->tab_route_planning);
}

void MainWindow::on_actionFlow_triggered() {
    ui->tabWidget->setCurrentWidget(ui->tab_flow);
}

void MainWindow::load_station_mapping() {
    TaskGetMapping *task = new TaskGetMapping(this);
    connect(task, &TaskGetMapping::result, [=]() {
        auto data = task->get_data();
        auto metros = task->get_metros();
        QMetaObject::invokeMethod(this, [=]() {
            this->station_mapping = data;
            this->metros = metros;
            for (auto &&mapping: station_mapping) {
                QString station = QString("(%3)%1 - %2")
                        .arg(mapping.stationID)
                        .arg(mapping.name)
                        .arg(mapping.lineID);
                ui->comboStation->addItem(station, mapping.stationID);
                ui->comboRouteFrom->addItem(station, mapping.stationID);
                ui->comboRouteTo->addItem(station, mapping.stationID);
            }
            update_route_map("A");
        });
    });
    scheduler.schedule(task);

}

void MainWindow::meteor_wizard_complete() {
    DB::init();
    load_station_mapping();
    scheduler.start();
}
