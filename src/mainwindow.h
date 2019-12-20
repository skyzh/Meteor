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
#include "TaskQueryFlow.h"
#include "TaskSmartTravel.h"
#include "MetroChartView.h"

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <QChartView>
#include <QLineSeries>
#include <QChart>
#include <QTableWidgetItem>
#include <QTimer>

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

    void begin_schedule();

private:
    Ui::MainWindow *ui;
    TaskScheduler scheduler;
    QProgressBar *schedulerProgress;
    QLabel *schedulerMessage;

    void setup_status_bar();

    void setup_chart(QList<QLineSeries *> series);

    QChart *chart;
    MetroChartView *chartView;
    MetroWidget *metroWidgetRoute;
    MetroWidget *metroWidgetFlow;
    MetroPainter metroRoutePainter;
    MetroPainter metroFlowPainter;

    QTimer *delayed_chart_update;

    QVector<TaskGetMapping::Mapping> station_mapping;
    QMap<QString, QList<qulonglong>> metros;

    QVector<QVector<QVector<unsigned long long>>> flow_result;
    QVector<unsigned long long> flow_time;
    QDateTime flow_date_time;
    int lst_flow_block;
    QMap<int, QString> action_msg;

    void load_station_mapping();

    void update_flow_position(int position, bool force_update = false);

    void update_route_map(QString line, long long highlight_station = -1, QMap<int, QString> msg = {});

    void update_passenger_chart();

    const int TABLE_STATION = Qt::UserRole + 1;
    const int TABLE_LINE = Qt::UserRole + 2;

private slots:

    void on_pushButtonRoutePlanning_clicked();

    void on_pushButtonQuery_clicked();

    void on_actionPassenger_Traffic_triggered();

    void on_actionRoute_Planning_triggered();

    void on_actionFlow_triggered();

    void on_pushButtonFlow_clicked();

    void on_sliderTime_sliderMoved(int position);

    void on_comboBoxFlow_currentIndexChanged(int index);

    void on_tableRoute_itemSelectionChanged();

    void on_radioButtonDaily_toggled(bool checked);

    void on_radioButtonHourly_toggled(bool checked);

    void on_radioButtonCustom_toggled(bool checked);

    void on_fromTime_dateTimeChanged(const QDateTime &dateTime);

    void on_comboLine_currentIndexChanged(int index);

    void on_comboStation_currentIndexChanged(int index);

    void on_pushButtonSmartTravel_clicked();

public slots:

    void tb_buttonTabQuery_clicked();

    void tb_buttonTabRoutePlanning_clicked();

    void tb_buttonTabFlow_clicked();

    void tb_sliderFlow_changed(int value);

    void meteor_wizard_complete();
};

#endif // MAINWINDOW_H
