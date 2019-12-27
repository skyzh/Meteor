//
// Created by Alex Chi on 2019/12/20.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

inline QString get_line_id(const QMap<QString, QList<qulonglong>> &metros, unsigned long long station) {
    for (auto &&lineID : metros.keys()) {
        auto &stations = metros[lineID];
        if (stations.count(station)) {
            return lineID;
        }
    }
    return "";
}

void MainWindow::update_route_info() {
    QMap<int, QString> action_msg;

    ui->tableRoute->clear();
    QList<QTableWidgetItem *> action_list;
    QString current_line;

    // [1] Board station
    {
        auto station_ = route_stations[0];
        auto board_item = new QTableWidgetItem(
                QString("Board at %1 Station")
                        .arg(station_mapping[station_].name));
        current_line = get_line_id(metros, station_);
        board_item->setData(TABLE_STATION, station_);
        board_item->setData(TABLE_LINE, current_line);
        action_list << board_item;

        action_msg[station_] = QString("Board");
    }

    auto lst_station_ = route_stations[0];

    for (int i = 1; i < route_stations.size(); i++) {
        auto station_ = route_stations[i];

        auto &stations = metros[current_line];

        // [2] Transfer Detection
        if (!stations.count(station_)) {
            auto lineID = get_line_id(metros, station_);
            auto transfer_item = new QTableWidgetItem(
                    QString("Transfer from %1 to %2 at %3 Station")
                            .arg(current_line)
                            .arg(lineID)
                            .arg(station_mapping[lst_station_].name));
            transfer_item->setData(TABLE_STATION, lst_station_);
            transfer_item->setData(TABLE_LINE, lineID);
            action_list << transfer_item;
            current_line = lineID;

            action_msg[lst_station_] = QString("Transfer to %1").arg(lineID);
        }

        // [3] Pass Station
        auto msg = QString("  %2%1 Station")
                .arg(station_mapping[station_].name);

        if (i < route_eta.size()) {
            if (route_eta[i]) {
                QDateTime timestamp;
                timestamp.setTime_t(route_eta[i]);
                msg = msg.arg(QString("(%1) ").arg(timestamp.toString("hh:mm")));
                if (!route_flow_time.empty()) {
                    auto iter = qLowerBound(route_flow_time.begin(), route_flow_time.end(), timestamp.toSecsSinceEpoch());
                    if (iter != route_flow_time.end()) {
                        long current_flow_block = iter - route_flow_time.begin();
                        auto pressure = route_flow_result[lst_station_][station_][current_flow_block];
                        auto pressure_index = std::min(pressure / 50000.0, 1.0);
                        if (pressure_index > 0.3) {
                            action_msg[station_] += QString(" %1% Crowded").arg(int(pressure_index * 100));
                        }
                    }
                }
            } else {
                msg = msg.arg("(__:__) ");
            }
        } else {
            msg = msg.arg("");
        }

        auto station_item = new QTableWidgetItem(msg);
        station_item->setData(TABLE_STATION, station_);
        station_item->setData(TABLE_LINE, current_line);

        action_list << station_item;

        lst_station_ = station_;
    }

    // [4] Exit Station
    {
        auto route_ = route_stations.last();
        auto exit_item = new QTableWidgetItem(
                QString("Exit at %1 Station")
                        .arg(station_mapping[route_].name));
        exit_item->setData(TABLE_STATION, route_);
        exit_item->setData(TABLE_LINE, current_line);
        action_list << exit_item;
        action_msg[route_] = QString("Exit");
    }

    // [5] Push into Table
    ui->tableRoute->setRowCount(action_list.length());
    ui->tableRoute->setColumnCount(1);
    for (int i = 0; i < action_list.length(); i++) {
        ui->tableRoute->setItem(i, 0, action_list[i]);
    }
    route_action_msg = action_msg;
    update_route_map(action_list[0]->data(TABLE_LINE).toString(), -1, action_msg);
}

void MainWindow::on_pushButtonRoutePlanning_clicked() {
    auto task = new TaskPlanRoute(this);
    task->args({ui->comboRouteFrom->currentData(), ui->comboRouteTo->currentData()});

    auto flow_task = new TaskQueryFlow(this);
    auto flow_date = ui->comboBoxSmartWeekday->currentData().toDate();
    auto flow_begin = QDateTime(flow_date).toSecsSinceEpoch();
    flow_task->args({flow_begin, flow_begin + 86400});

    auto smart_travel_task = new TaskQuerySmartTravel(this);
    auto departure_time = QDateTime(flow_date, ui->timeEditDeparture->time());
    smart_travel_task->args({departure_time.toSecsSinceEpoch()});

    route_flow_result = {};
    route_flow_time = {};
    route_eta = {};

    connect(task, &TaskPlanRoute::result, [=]() {
        if (station_mapping.empty()) return;

        route_stations = task->get_data();

        QMetaObject::invokeMethod(this, [=]() {
            update_route_info();

            // Meteor Adviser
            if (ui->checkBoxAdviser->isChecked()) {
                connect(flow_task, &TaskQueryFlow::result, [=]() {
                    route_flow_result = flow_task->get_flow_per_hour_result();
                    route_flow_time = flow_task->get_flow_time();
                    QMetaObject::invokeMethod(this, [=]() { update_route_info(); });
                });

                smart_travel_task->route_stations = route_stations;

                connect(smart_travel_task, &TaskQuerySmartTravel::result, [=]() {
                    auto eta = smart_travel_task->get_estimated_time();
                    for (auto &&e:eta) {
                        if (e != 0) route_eta << departure_time.addSecs(e).toSecsSinceEpoch();
                        else route_eta << 0;
                    }

                    QMetaObject::invokeMethod(this, [=]() { update_route_info(); });
                });

                scheduler.schedule(smart_travel_task);
                scheduler.schedule(flow_task);
            }
        });
    });

    scheduler.schedule(task);
}


void MainWindow::update_route_map(QString line, long long highlight_station, QMap<int, QString> msg) {
    QVector<MetroStation> stations;
    QVector<MetroSegment> segments;
    qreal c_x = 0, c_y = 0;
    int q = 0;
    for (auto &_station : metros[line]) {
        auto &mapping = station_mapping[_station];
        qreal x = (q++) * 50;
        qreal y = 0;
        if (mapping.stationID == highlight_station) {
            c_x = x;
            c_y = y;
        }
        auto station = MetroStation{
                mapping.name,
                mapping.stationID,
                x,
                y,
                line,
                msg[mapping.stationID]
        };
        if (!stations.empty()) {
            auto lst_station = stations.last();
            segments << MetroSegment{
                    lst_station.x, lst_station.y,
                    station.x, station.y,
                    MetroPainter::line_color_mapping(line),
                    MetroPainter::line_color_mapping(line)
            };
        }
        stations << station;
    }
    for (auto &&station:stations) {
        if (station.id == highlight_station) {
            station.lineID = "";
        }
    }
    metroWidgetRoute->setStations(stations, segments);
    metroWidgetRoute->set_camera_pos(c_x, c_y);
}

void MainWindow::on_tableRoute_itemSelectionChanged() {
    auto item = ui->tableRoute->currentItem();
    update_route_map(
            item->data(TABLE_LINE).toString(),
            item->data(TABLE_STATION).toLongLong(),
            route_action_msg
    );
}
