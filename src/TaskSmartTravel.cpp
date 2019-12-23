//
// Created by Alex Chi on 2019/12/08.
//

#include "TaskSmartTravel.h"
#include "TaskPlanRoute.h"
#include <QDebug>

TaskSmartTravel::TaskSmartTravel(QObject *parent) : TaskBaseFlowAnalysis(parent) {

}

bool TaskSmartTravel::journal() {
    return true;
}

QString TaskSmartTravel::name() {
    return "smart_travel_" + TaskBaseFlowAnalysis::name();
}

QString TaskSmartTravel::display_name() {
    return "Smart Travel " + TaskBaseFlowAnalysis::display_name();
}

TaskSmartTravel::~TaskSmartTravel() {

}

bool TaskSmartTravel::init_flow_data(QSqlQuery &q) {
    auto mat = TaskPlanRoute::get_route_mapping();
    N = mat.size();

    init_flow_time(flow_time, start_time, end_time, time_div);
    init_flow_matrix(flow_sum, N, start_time, end_time, time_div);
    init_flow_matrix(flow_n, N, start_time, end_time, time_div);

    emit message("Purging data");

    q.prepare("delete from smart_travel where start_time=:start_time");

    q.bindValue(":start_time", start_time);

    if (!q.exec()) {
        emit_sql_error("Error when purging data", q);
        return false;
    }

    return true;
}

bool TaskSmartTravel::commit_to_database(QSqlQuery &q) {
    emit message("Committing to database");

    const QString INSERT_QUERY = "insert into smart_travel (start_time, enter_station_id, exit_station_id, time_block, flow_sum, flow_n) values (?,?,?,?,?,?)";

    q.prepare(INSERT_QUERY);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int _size = flow_n[i][j].size();
            for (int tb = 0; tb < _size; tb++) {
                if (flow_n[i][j][tb] != 0) {
                    q.bindValue(0, start_time);
                    q.bindValue(1, i);
                    q.bindValue(2, j);
                    q.bindValue(3, tb);
                    q.bindValue(4, flow_sum[i][j][tb]);
                    q.bindValue(5, flow_n[i][j][tb]);
                    if (!q.exec()) {
                        emit_sql_error("Error while updating table", q);
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
