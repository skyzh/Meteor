#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TaskQueryEntryExit.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QValueAxis>
#include <QDateTimeAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scheduler(this)
{
    ui->setupUi(this);
    setup_status_bar();
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

void MainWindow::on_pushButton_clicked() {
    scheduler.schedule("read_dataset");
    QDateTime start, end;
    start.setDate(QDate(2019, 1, 9));
    end.setDate(QDate(2019, 1, 10));
    TaskQueryEntryExit* task = new TaskQueryEntryExit(this);
    task->args({ 3600, start.toSecsSinceEpoch(), end.toSecsSinceEpoch() });
    connect(task, &TaskQueryEntryExit::result, [=] () {
        QMutexLocker l(&task->_data_mutex);
        this->data = task->data;
        QMetaObject::invokeMethod(this, &MainWindow::update_chart);
    });
    scheduler.schedule(task);
}

void MainWindow::setup_status_bar() {
    schedulerMessage = new QLabel(this);
    schedulerProgress = new QProgressBar(this);
    schedulerProgress->setMinimum(0);
    schedulerProgress->setMaximum(100);
    ui->statusbar->addPermanentWidget(schedulerProgress, 1);
    ui->statusbar->addPermanentWidget(schedulerMessage, 0);
}

void MainWindow::setup_chart(QLineSeries* series) {
    ui->verticalLayout->removeWidget(chartView);
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("MM-dd");
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Count");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    chart->setTitle("In/Out of Station");
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout->addWidget(chartView);
}

void MainWindow::update_chart() {
    QLineSeries* series = new QLineSeries;
    foreach(auto record, data) {
        if (record.status == 0)
            series->append(record.timestamp, record.count);
    }
    qDebug() << series;
    this->setup_chart(series);
}
