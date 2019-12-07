#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QValueAxis>
#include <QDateTimeAxis>
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

    metroWidgetRoute = new MetroWidget(&metroRoutePainter, this);
    metroWidgetFlow = new MetroWidget(&metroFlowPainter, this);
    ui->layoutRoute->addWidget(metroWidgetRoute);
    ui->layoutFlow->addWidget(metroWidgetFlow);
    lst_flow_block = -1;

    connect(&scheduler, &TaskScheduler::progress, this, &MainWindow::progress);
    connect(&scheduler, &TaskScheduler::message, this, &MainWindow::message);

    schedulerProgress->setValue(0);
    scheduler.start();

    load_station_mapping();
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

    qreal xx = 0;

    for (auto _series : series) {
        chart->addSeries(_series);
        _series->attachAxis(axisX);
        _series->attachAxis(axisY);
        for (auto &&d : _series->points()) xx = qMax(xx, d.y());
    }

    axisY->setRange(0, xx);

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
        QVector<TaskQueryEntryExit::EntryExitResult> data = task->get_data();

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
        QVector<TaskGetMapping::Mapping> data = task->get_data();

        QMetaObject::invokeMethod(this, [=]() {
            this->station_mapping = data;
            QVector<MetroStation> stations;
            QVector<MetroSegment> segments;
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
                    auto station = MetroStation{
                            mapping.name,
                            mapping.stationID,
                            (q++) * 150.0,
                            0,
                            mapping.lineID
                    };
                    if (!stations.empty()) {
                        auto lst_station = stations.last();
                        segments << MetroSegment{
                                lst_station.x, lst_station.y,
                                station.x, station.y,
                                MetroPainter::line_color_mapping(station.lineID),
                                {}
                        };
                    }
                    stations << station;
                }
            }
            metroWidgetRoute->setStations(stations, segments);
        });
    });
    scheduler.schedule(task);

}

void MainWindow::on_pushButtonFlow_clicked() {
    TaskFlowAnalysis *task = new TaskFlowAnalysis(this);
    task->args({1546995600, 1546995600 + 86400, 60});
    connect(task, &TaskFlowAnalysis::result, [=]() {
        auto flow_result = task->get_flow_result();
        auto flow_time = task->get_flow_time();

        QMetaObject::invokeMethod(this, [=]() {
            this->flow_result = flow_result;
            this->flow_time = flow_time;
            lst_flow_block = -1;
        });
    });
    scheduler.schedule(task);

}

void MainWindow::on_flowTime_dateTimeChanged(const QDateTime &dateTime) {
    if (!station_mapping.empty()) {
        if (flow_time.empty()) return;
        int current_flow_block = qLowerBound(flow_time, dateTime.toSecsSinceEpoch()) - flow_time.begin();
        if (current_flow_block >= flow_time.size()) return;
        if (current_flow_block == lst_flow_block) return;
        lst_flow_block = current_flow_block;
        QVector<MetroStation> stations;
        QVector<MetroSegment> segments;
        int q = 0;
        for (auto &mapping : station_mapping) {
            if (mapping.lineID == "A") {
                auto station = MetroStation{
                        mapping.name,
                        mapping.stationID,
                        (q++) * 150.0,
                        0,
                        mapping.lineID
                };
                if (!stations.empty()) {
                    auto lst_station = stations.last();
                    segments << MetroSegment{
                            lst_station.x, lst_station.y,
                            station.x, station.y,
                            Qt::black,
                            QString("%1").arg(flow_result[lst_station.id][station.id][current_flow_block])
                    };
                }
                stations << station;
            }
        }
        metroWidgetFlow->setStations(stations, segments);
    }
}
