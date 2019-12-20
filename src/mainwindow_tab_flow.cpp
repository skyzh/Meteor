//
// Created by Alex Chi on 2019/12/20.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TouchBar.h"

inline qreal map_to_line_color(qreal flow) {
    const qreal MAX_FLOW = 30000;
    if (flow >= MAX_FLOW) return 0;
    if (flow <= 0) return 1.0 / 3;
    return (MAX_FLOW - flow) / MAX_FLOW / 3.0;
}

void MainWindow::update_flow_position(int position, bool force_update) {
    if (position >= 86400) position = 86400 - 1;
    auto _slide_time = QTime(4, 0).addSecs(position);
    ui->flowTime->setTime(_slide_time);

    auto slide_time = flow_date_time.addSecs(4 * 60 * 60 + position).toSecsSinceEpoch();

    const QString station_selected = ui->comboBoxFlow->currentData().toString();

    if (!station_mapping.empty()) {
        if (flow_time.empty()) return;
        auto iter = qLowerBound(flow_time.begin(), flow_time.end(), slide_time);
        if (iter == flow_time.end()) return;
        long current_flow_block = iter - flow_time.begin();
        if (current_flow_block >= flow_time.size()) return;
        if (!force_update && current_flow_block == lst_flow_block) return;
        lst_flow_block = current_flow_block;
        QVector<MetroStation> stations;
        QVector<MetroSegment> segments;
        int q = 0;
        for (auto &_station : metros[station_selected]) {
            auto &mapping = station_mapping[_station];
            auto station = MetroStation{
                    mapping.name,
                    mapping.stationID,
                    (q++) * 80.0,
                    0,
                    station_selected
            };
            if (!stations.empty()) {
                auto lst_station = stations.last();
                auto &flow_in = flow_result[lst_station.id][station.id];
                auto &flow_out = flow_result[station.id][lst_station.id];
                auto flow_size_in = flow_result[lst_station.id][station.id][current_flow_block];
                auto flow_size_out = flow_result[station.id][lst_station.id][current_flow_block];

                segments << MetroSegment{
                        lst_station.x, lst_station.y,
                        station.x, station.y,
                        QColor::fromHslF(map_to_line_color(flow_size_in), 0.7, 0.5),
                        QColor::fromHslF(map_to_line_color(flow_size_out), 0.7, 0.5),
                        QString::number(flow_size_in),
                        QString::number(flow_size_out)
                };
            }
            stations << station;
        }
        metroWidgetFlow->setStations(stations, segments);
    }

}

void MainWindow::on_pushButtonFlow_clicked() {
    auto task = new TaskQueryFlow(this);
    flow_date_time = ui->flowDate->dateTime();
    auto flow_begin = flow_date_time.toSecsSinceEpoch();
    task->args({flow_begin, flow_begin + 86400});
    connect(task, &TaskQueryFlow::result, [=]() {
        auto flow_result = task->get_flow_per_hour_result();
        auto flow_time = task->get_flow_time();

        QMetaObject::invokeMethod(this, [=]() {
            this->flow_result = flow_result;
            this->flow_time = flow_time;
            lst_flow_block = -1;
            update_flow_position(ui->sliderTime->value(), true);
        });

    });
    scheduler.schedule(task);
}


void MainWindow::on_sliderTime_sliderMoved(int position) {
    setTouchbarSliderFlowValue(position);
    update_flow_position(position);
}

void MainWindow::on_comboBoxFlow_currentIndexChanged(int index) {
    update_flow_position(ui->sliderTime->value(), true);
}
