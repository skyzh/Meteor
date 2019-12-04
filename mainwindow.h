#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TaskReadDataset.h"
#include "TaskScheduler.h"
#include "TaskQueryEntryExit.h"

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

    void on_pushButton_clicked();

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TaskScheduler scheduler;
    QProgressBar *schedulerProgress;
    QLabel *schedulerMessage;

    void setup_status_bar();

    void setup_chart(QLineSeries *series);

    QList<TaskQueryEntryExit::EntryExitResult> data;
    QChart *chart;
    QChartView *chartView;

private slots:

    void update_chart();
};

#endif // MAINWINDOW_H
