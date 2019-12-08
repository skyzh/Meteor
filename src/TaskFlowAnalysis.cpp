//
// Created by Alex Chi on 2019/12/06.
//

#include "TaskBaseFlowAnalysis.h"
#include "TaskFlowAnalysis.h"
#include "TaskPlanRoute.h"

bool TaskFlowAnalysis::journal() {
    return true;
}

QString TaskFlowAnalysis::name() {
    return "flow_analysis_" + TaskBaseFlowAnalysis::name();
}

QString TaskFlowAnalysis::display_name() {
    return "Flow Analysis " + TaskBaseFlowAnalysis::display_name();
}

TaskFlowAnalysis::~TaskFlowAnalysis() {

}

bool TaskFlowAnalysis::init_flow_data(QSqlQuery& q) {
    emit message("Planning per-user route");
    auto mat = TaskPlanRoute::get_route_mapping();
    N = mat.size();

    route_cache.clear();
    for (int i = 0; i < N; i++) {
        route_cache.push_back({});
        for (int j = 0; j < N; j++) {
            route_cache[i].push_back(TaskPlanRoute::plan_route(mat, N, i, j));
        }
    }

    init_flow_time(flow_time, start_time, end_time, time_div);
    init_flow_matrix(flow, N, start_time, end_time, time_div);

    emit message("Purging data");

    q.prepare("delete from flow where start_time=:start_time");

    q.bindValue(":start_time", start_time);

    if (!q.exec()) {
        emit_sql_error("Error when purging data", q);
        return false;
    }
    return true;
}

bool TaskFlowAnalysis::process_flow_data(const FlowResult &flow_) {
    qulonglong enter_time_block = (flow_.enter_time - start_time) / time_div,
            exit_time_block = (flow_.exit_time - start_time) / time_div;
    qulonglong time_block_cnt = exit_time_block - enter_time_block;
    qulonglong lst_time_block = enter_time_block;
    auto &route = route_cache[flow_.enter_station][flow_.exit_station];
    int route_size = route.size() - 1;
    for (int i = 0; i < route_size; i++) {
        qulonglong current_time_block = double(i) / route_size * time_block_cnt + enter_time_block;
        auto route_enter = route[i];
        auto route_exit = route[i + 1];
        for (qulonglong tb = lst_time_block; tb <= current_time_block; tb++) {
            ++flow[route_enter][route_exit][tb];
        }
        lst_time_block = current_time_block;
    }
    return true;
}

bool TaskFlowAnalysis::commit_to_database(QSqlQuery &q) {
    emit message("Committing to database");

    const QString INSERT_QUERY = "insert into flow (start_time, enter_station_id, exit_station_id, time_block, flow) values (?,?,?,?,?)";

    q.prepare(INSERT_QUERY);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int _size = flow[i][j].size();
            for (int tb = 0; tb < _size; tb++) {
                if (flow[i][j][tb] != 0) {
                    q.bindValue(0, start_time);
                    q.bindValue(1, i);
                    q.bindValue(2, j);
                    q.bindValue(3, tb);
                    q.bindValue(4, flow[i][j][tb]);
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

TaskFlowAnalysis::TaskFlowAnalysis(QObject *parent) : TaskBaseFlowAnalysis(parent) {}
