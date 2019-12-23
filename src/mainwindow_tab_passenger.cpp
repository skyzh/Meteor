//
// Created by Alex Chi on 2019/12/20.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QValueAxis>
#include <QDateTimeAxis>
#include <QGraphicsLayout>

void MainWindow::update_passenger_chart() {
    if (scheduler.running()) return;
    long long start = ui->fromTime->dateTime().toSecsSinceEpoch();
    long long end = ui->toTime->dateTime().toSecsSinceEpoch();
    long long time_div = ui->lineEditTimestep->text().toULongLong();
    if (time_div == 0) return;
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
            QSplineSeries *in_series = new QSplineSeries;
            QSplineSeries *out_series = new QSplineSeries;

            for (auto &&record : data) {
                if (record.status == 1)
                    in_series->append(record.timestamp * 1000, record.count * (3600 / time_div));
                if (record.status == 0)
                    out_series->append(record.timestamp * 1000, record.count * (3600 / time_div));
            }
            in_series->setName("Entry per hour");
            out_series->setName("Exit per hour");
            setup_chart({in_series, out_series});
        });
    });
    scheduler.schedule(task);
}

void MainWindow::on_radioButtonDaily_toggled(bool checked) {
    if (checked) {
        ui->toTime->setDateTime(ui->fromTime->dateTime().addSecs(86400));
        ui->lineEditTimestep->setText("720");
    }
}

void MainWindow::on_radioButtonHourly_toggled(bool checked) {
    if (checked) {
        ui->toTime->setDateTime(ui->fromTime->dateTime().addSecs(3600));
        ui->lineEditTimestep->setText("30");
    }
}

void MainWindow::on_radioButtonCustom_toggled(bool checked) {
    if (checked) {
        ui->toTime->setEnabled(true);
        ui->lineEditTimestep->setEnabled(true);
    } else {
        ui->toTime->setEnabled(false);
        ui->lineEditTimestep->setEnabled(false);
    }
}

void MainWindow::on_fromTime_dateTimeChanged(const QDateTime &dateTime) {
    if (ui->radioButtonDaily->isChecked()) {
        ui->toTime->setDateTime(dateTime.addSecs(86400));
    }
    if (ui->radioButtonHourly->isChecked()) {
        ui->toTime->setDateTime(dateTime.addSecs(3600));
    }
}

void MainWindow::on_comboLine_currentIndexChanged(int index) {
    if (ui->comboLine->currentData() == "All") {
        ui->comboStation->setEnabled(true);
    } else {
        ui->comboStation->setEnabled(false);
    }
}

void MainWindow::on_comboStation_currentIndexChanged(int index) {
    if (ui->comboStation->currentData() == "All") {
        ui->comboLine->setEnabled(true);
    } else {
        ui->comboLine->setEnabled(false);
    }
}


void MainWindow::setup_chart(QList<QSplineSeries *> series) {
    if (chartView) ui->layoutChart->removeWidget(chartView);
    chart = new QChart();
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

    chartView = new MetroChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->layoutChart->addWidget(chartView);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignTop);

    connect(chartView, &MetroChartView::request_data, this, [=]() {
        ui->fromTime->setDateTime(axisX->min());
        ui->toTime->setDateTime(axisX->max());
        delayed_chart_update->stop();
        delayed_chart_update->start(500);
    });
}

void MainWindow::on_pushButtonQuery_clicked() {
    update_passenger_chart();
}
