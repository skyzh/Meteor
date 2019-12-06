#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TaskQueryEntryExit.h"
#include "TaskPlanRoute.h"

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
    ui->tabWidget->setAutoFillBackground(true);
    ui->tab_passenger_traffic->setAutoFillBackground(true);
    setup_status_bar();
    {
        QStringList list;
        for (int i = 0; i <= 80; i++) {
            list << QString("%1").arg(i);
        }
        ui->comboStation->addItems(list);
        ui->comboRouteFrom->addItems(list);
        ui->comboRouteTo->addItems(list);
    }
    {
        QStringList list;
        for (int i = 'A'; i <= 'C'; i++) {
            list << QString(i);
        }
        ui->comboLine->addItems(list);
    }

    connect(&scheduler, &TaskScheduler::progress, this, &MainWindow::progress);
    connect(&scheduler, &TaskScheduler::message, this, &MainWindow::message);
    scheduler.start();
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
    task->args({ui->comboRouteFrom->currentText(), ui->comboRouteTo->currentText()});
    scheduler.schedule(task);

    connect(task, &TaskPlanRoute::result, [=]() {
        task->_data_mutex.lock();
        QList <qulonglong> route = task->data;
        task->_data_mutex.unlock();

        QMetaObject::invokeMethod(this, [=]() {
            QString str;
            for (auto i : route) str += QString("%1 ->").arg(i);
            ui->textRoute->setPlainText(str);
        });
    });
}

void MainWindow::on_pushButtonQuery_clicked() {
    long long start = ui->fromTime->dateTime().toSecsSinceEpoch();
    long long end = ui->toTime->dateTime().toSecsSinceEpoch();
    long long time_div = 3600;
    if (end - start <= 60 * 60 * 12) time_div = 1800;
    if (end - start <= 60 * 60 * 1) time_div = 60;
    if (end - start <= 60) time_div = 1;
    TaskQueryEntryExit *task = new TaskQueryEntryExit(this);
    task->args({
                       time_div,
                       start,
                       end,
                       ui->comboLine->currentText(),
                       ui->comboStation->currentText()
               });

    connect(task, &TaskQueryEntryExit::result, [=]() {
        task->_data_mutex.lock();
        QList<TaskQueryEntryExit::EntryExitResult> data = task->data;
        task->_data_mutex.unlock();

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
