//
// Created by Alex Chi on 2019/12/20.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_pushButtonRoutePlanning_clicked() {
    auto task = new TaskPlanRoute(this);
    task->args({ui->comboRouteFrom->currentData(), ui->comboRouteTo->currentData()});
    scheduler.schedule(task);

    connect(task, &TaskPlanRoute::result, [=]() {
        if (station_mapping.empty()) return;

        QList<qulonglong> route = task->get_data();

        if (route.size() == 0) return;

        QMetaObject::invokeMethod(this, [=]() {
            ui->tableRoute->clear();
            QList<QTableWidgetItem *> action_list;
            QString current_line;
            {
                auto route_ = route[0];
                auto board_item = new QTableWidgetItem(
                        QString("Board at %1 Station")
                                .arg(station_mapping[route_].name));
                board_item->setData(TABLE_STATION, route_);
                for (auto &&lineID : metros.keys()) {
                    auto &stations = metros[lineID];
                    if (stations.count(route_)) {
                        current_line = lineID;
                        break;
                    }
                }
                board_item->setData(TABLE_LINE, current_line);
                action_msg[route_] = QString("Board");
                action_list << board_item;
            }

            QMap<int, QString> action_msg;

            auto lst_route = route[0];

            for (int i = 1; i < route.size(); i++) {
                auto route_ = route[i];

                auto &stations = metros[current_line];
                if (!stations.count(route_)) {
                    for (auto &&lineID : metros.keys()) {
                        auto &stations = metros[lineID];
                        if (stations.count(route_)) {
                            auto transfer_item = new QTableWidgetItem(
                                    QString("Transfer from %1 to %2 at %3 Station")
                                            .arg(current_line)
                                            .arg(lineID)
                                            .arg(station_mapping[lst_route].name));
                            transfer_item->setData(TABLE_STATION, lst_route);
                            transfer_item->setData(TABLE_LINE, lineID);
                            action_list << transfer_item;
                            current_line = lineID;
                            action_msg[lst_route] = QString("Transfer to %1").arg(lineID);
                            break;
                        }
                    }
                }

                auto station_item = new QTableWidgetItem(
                        QString("  %1 Station")
                                .arg(station_mapping[route_].name));
                station_item->setData(TABLE_STATION, route_);
                station_item->setData(TABLE_LINE, current_line);
                action_list << station_item;

                lst_route = route_;
            }
            {
                auto route_ = route.last();
                auto exit_item = new QTableWidgetItem(
                        QString("Exit at %1 Station")
                                .arg(station_mapping[route_].name));
                exit_item->setData(TABLE_STATION, route_);
                exit_item->setData(TABLE_LINE, current_line);
                action_list << exit_item;
                action_msg[route_] = QString("Exit");
            }
            ui->tableRoute->setRowCount(action_list.length());
            ui->tableRoute->setColumnCount(1);
            for (int i = 0; i < action_list.length(); i++) {
                ui->tableRoute->setItem(i, 0, action_list[i]);
            }
            this->action_msg = action_msg;
            update_route_map(action_list[0]->data(TABLE_LINE).toString(), -1, action_msg);
        });
    });
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
            action_msg
    );
}
