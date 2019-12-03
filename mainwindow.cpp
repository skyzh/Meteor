#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TaskQuery.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scheduler(this)
{
    ui->setupUi(this);
    setup_status_bar();
    setup_chart();
    connect(&scheduler, &TaskScheduler::progress, this, &MainWindow::progress);
    connect(&scheduler, &TaskScheduler::message, this, &MainWindow::message);
    scheduler.start();
    scheduler.schedule("read_dataset");
}

MainWindow::~MainWindow()
{
    scheduler.quit();
    scheduler.wait();
    delete ui;
}
void MainWindow::progress(qreal percent) {
    schedulerProgress->setValue(int(percent * 100));
}

void MainWindow::message(QString msg) {
    schedulerMessage->setText(msg);
}

void MainWindow::on_pushButton_clicked() {
    scheduler.schedule("read_dataset");
    QDateTime start, end;
    start.setDate(QDate(2019, 1, 9));
    end.setDate(QDate(2019, 1, 10));
    start.toSecsSinceEpoch();
    scheduler.schedule("query_entry_exit");
    chart->removeSeries(series);
    chart->addSeries(series);
}

void MainWindow::setup_status_bar() {
    schedulerMessage = new QLabel(this);
    schedulerProgress = new QProgressBar(this);
    schedulerProgress->setMinimum(0);
    schedulerProgress->setMaximum(100);
    ui->statusbar->addPermanentWidget(schedulerProgress, 1);
    ui->statusbar->addPermanentWidget(schedulerMessage, 0);
}

void MainWindow::setup_chart() {
    series = new QLineSeries();
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple line chart example");
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout->addWidget(chartView);
}
