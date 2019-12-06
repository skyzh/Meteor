#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TaskReadDataset.h"
#include "TaskScheduler.h"
#include "TaskQueryEntryExit.h"
#include "MetroWidget.h"
#include "MetroPainter.h"
#include "TaskPlanRoute.h"
#include "TaskGetMapping.h"
#include "TaskFlowAnalysis.h"

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <QChartView>
#include <QLineSeries>
#include <QChart>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

private slots:

    void progress(qreal percent);

    void message(QString msg);

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TaskScheduler scheduler;
    QProgressBar *schedulerProgress;
    QLabel *schedulerMessage;

    void setup_status_bar();

    void setup_chart(QList<QLineSeries *> series);

    QChart *chart;
    QChartView *chartView;
    MetroWidget *metroWidget;
    MetroPainter metroPainter;

    QList<TaskGetMapping::Mapping> station_mapping;

    void load_station_mapping();

private slots:

    void on_pushButtonRoutePlanning_clicked();

    void on_pushButtonQuery_clicked();

    void on_actionPassenger_Traffic_triggered();

    void on_actionRoute_Planning_triggered();

    void on_actionFlow_triggered();

    void on_pushButtonFlow_clicked();

public slots:

    void tb_buttonTabQuery_clicked();

    void tb_buttonTabRoutePlanning_clicked();

    void tb_buttonTabFlow_clicked();
};

#endif // MAINWINDOW_H
