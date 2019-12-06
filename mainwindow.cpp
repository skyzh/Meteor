#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QSizePolicy>
#include <QGraphicsLayout>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), scheduler(this) {
    ui->setupUi(this);
    setup_status_bar();
    ui->comboStation->addItem("All", "All");
    {
        QStringList list;
        list << "B - Line 1"; // 202 45 92
        list << "A - Line 4"; // 130 192 90
        list << "C - Line 2"; // 240 136 62
        ui->comboLine->addItem("All", "All");
        ui->comboLine->addItem("A - Line 4", "A");
        ui->comboLine->addItem("B - Line 1", "B");
        ui->comboLine->addItem("C - Line 2", "C");
    }

    metroWidget = new MetroWidget(&metroPainter, this);
    ui->layoutRoute->addWidget(metroWidget);

    connect(&scheduler, &TaskScheduler::progress, this, &MainWindow::progress);
    connect(&scheduler, &TaskScheduler::message, this, &MainWindow::message);

    schedulerProgress->setValue(0);
    scheduler.start();

    load_station_mapping();
}

MainWindow::~MainWindow() {
    scheduler.quit();
    scheduler.wait();
    ui->layoutRoute->removeWidget(metroWidget);
    delete metroWidget;
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

void MainWindow::setup_chart(QList<QLineSeries *> series) {
    if (chartView) ui->layoutChart->removeWidget(chartView);
    chart = new QChart();
    chart->legend()->hide();
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(12);
    axisX->setFormat("MM-dd HH:mm");
    axisX->setLabelsAngle(-90);
    chart->addAxis(axisX, Qt::AlignBottom);
    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    chart->addAxis(axisY, Qt::AlignLeft);

    for (auto _series : series) {
        chart->addSeries(_series);
        _series->attachAxis(axisX);
        _series->attachAxis(axisY);
    }

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->layoutChart->addWidget(chartView);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
}

void MainWindow::on_pushButtonRoutePlanning_clicked() {
    auto task = new TaskPlanRoute(this);
    task->args({ui->comboRouteFrom->currentData(), ui->comboRouteTo->currentData()});
    scheduler.schedule(task);

    connect(task, &TaskPlanRoute::result, [=]() {
        QList<qulonglong> route = task->get_data();

        QMetaObject::invokeMethod(this, [=]() {
            QString str;
            for (auto i : route) str += QString("%1 ->").arg(i);
            // ui->textRoute->setPlainText(str);
        });
    });
}

void MainWindow::on_pushButtonQuery_clicked() {
    long long start = ui->fromTime->dateTime().toSecsSinceEpoch();
    long long end = ui->toTime->dateTime().toSecsSinceEpoch();
    long long time_div = 3600;
    if (end - start <= 60 * 60 * 12) time_div = 300;
    if (end - start <= 60 * 60 * 1) time_div = 30;
    if (end - start <= 60) time_div = 1;
    TaskQueryEntryExit *task = new TaskQueryEntryExit(this);
    task->args({
                       time_div,
                       start,
                       end,
                       ui->comboLine->currentData(),
                       ui->comboStation->currentData()
               });

    connect(task, &TaskQueryEntryExit::result, [=]() {
        QList<TaskQueryEntryExit::EntryExitResult> data = task->get_data();

        QMetaObject::invokeMethod(this, [=]() {
            QLineSeries *in_series = new QLineSeries;
            QLineSeries *out_series = new QLineSeries;

            for (auto &&record : data) {
                if (record.status == 0)
                    in_series->append(record.timestamp * 1000, record.count);
                if (record.status == 1)
                    out_series->append(record.timestamp * 1000, record.count);
            }
            setup_chart({in_series, out_series});
        });
    });
    scheduler.schedule(task);
}

void MainWindow::tb_buttonTabQuery_clicked() {
    ui->tabWidget->setCurrentWidget(ui->tab_passenger_traffic);
}

void MainWindow::tb_buttonTabRoutePlanning_clicked() {
    ui->tabWidget->setCurrentWidget(ui->tab_route_planning);
}

void MainWindow::tb_buttonTabFlow_clicked() {
    ui->tabWidget->setCurrentWidget(ui->tab_flow);
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
        QList<TaskGetMapping::Mapping> data = task->get_data();

        QMetaObject::invokeMethod(this, [=]() {
            this->station_mapping = data;
            QList<MetroStation> stations;
            int q = 0;
            for (auto &&mapping: data) {
                QString station = QString("(%3)%1 - %2")
                        .arg(mapping.stationID)
                        .arg(mapping.name)
                        .arg(mapping.lineID);
                ui->comboStation->addItem(station, mapping.stationID);
                ui->comboRouteFrom->addItem(station, mapping.stationID);
                ui->comboRouteTo->addItem(station, mapping.stationID);
                if (mapping.lineID == "A") {
                    stations << MetroStation {
                        mapping.name,
                        mapping.stationID,
                        (q++) * 150.0,
                        0
                    };
                }
            }
            metroWidget->setStations(stations);
        });
    });
    scheduler.schedule(task);

}
