//
// Created by Alex Chi on 2019/12/06.
//

#include "db.h"
#include "TaskFlowAnalysis.h"
#include "TaskReadDataset.h"
#include "TaskPlanRoute.h"
#include "utils.h"

#include <QDateTime>
#include <QDebug>

TaskFlowAnalysis::TaskFlowAnalysis(QObject *parent) : Task(parent) {

}

void TaskFlowAnalysis::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Open SQL connection", db);
        return;
    }

    QSqlQuery q(db);

    QString QUERY = "select * from dataset "
                    "where :start_time <= time and time < :end_time "
                    "order by time asc, status desc";

    q.prepare(QUERY);

    {
        qulonglong failed_attempts = 0;
        const qulonglong TIME_STEP = 3600;

        QMutexLocker l(&_data_mutex);
        QMap<QString, FlowResult> mapping;

        // [1] Initialize Flow Vector
        emit message("Planning per-user route");
        init_flow_data();

        emit progress(0);
        // [2] Fetch Data from Database
        for (qulonglong time = start_time; time < end_time; time += TIME_STEP) {
            emit message(get_timestamp_str(time));
            q.bindValue(":start_time", time);
            q.bindValue(":end_time", time + TIME_STEP);
            if (!q.exec()) {
                emit_sql_error("Query Error", q);
                return;
            }

            while (q.next()) {
                int status = q.value(4).toULongLong();
                QString userID = q.value(5).toString().toLower();
                unsigned long long stationID = q.value(2).toULongLong();
                unsigned long long time = q.value(0).toULongLong();
                if (stationID >= N) {
                    emit message("Invalid station ID");
                    emit success(false);
                    return;
                }
                if (status == 1) {
                    mapping[userID] = FlowResult{
                            time, 0,
                            stationID, 0,
                            userID
                    };
                }
                if (status == 0) {
                    if (mapping.count(userID) == 0) {
                        ++failed_attempts;
                    } else {
                        FlowResult flow_ = mapping[userID];
                        mapping.remove(userID);
                        flow_.exit_time = time;
                        flow_.exit_station = stationID;
                        data << flow_;

                        // [3] Process Flow Record
                        process_flow_data(flow_);
                    }
                }
            }
            if (_cancel) break;
            emit progress(double(time - start_time) / (end_time - start_time));
        }

        emit message("Post-processing");

        // [4] Process Flow per hour
        if (!_cancel) postprocess_flow_data();
    }

    if (_cancel) {
        emit success(false);
    } else {
        emit result();
        emit success(true);
    }

    db.close();
}

QString TaskFlowAnalysis::name() {
    return "flow_analysis";
}

QList<Task *> TaskFlowAnalysis::dependencies() {
    return TaskReadDataset::from_time_range(start_time, end_time, this);
}

QString TaskFlowAnalysis::display_name() {
    return "Flow Analysis";
}

TaskFlowAnalysis::~TaskFlowAnalysis() {

}

bool TaskFlowAnalysis::parse_args() {
    {
        QDateTime timestamp;
        timestamp.setTime_t(get_arg(0).toUInt());
        timestamp.setTime(QTime(0, 0));
        start_time = timestamp.toSecsSinceEpoch();
    }
    {
        QDateTime timestamp;
        timestamp.setTime_t(get_arg(1).toUInt());
        timestamp.setTime(QTime(0, 0));
        end_time = timestamp.toSecsSinceEpoch();
    }
    time_div = get_arg(2).toULongLong();
    return true;
}

void TaskFlowAnalysis::init_flow_data() {
    auto mat = TaskPlanRoute::get_route_mapping();
    N = mat.size();
    route_cache.clear();
    for (int i = 0; i < N; i++) {
        route_cache.push_back({});
        for (int j = 0; j < N; j++) {
            route_cache[i].push_back(TaskPlanRoute::plan_route(mat, N, i, j));
        }
    }

    flow.clear();
    flow_time.clear();
    for (qulonglong time = start_time; time <= end_time; time += time_div) {
        flow_time << time;
    }
    for (int i = 0; i < N; i++) {
        flow.push_back({});
        for (int j = 0; j < N; j++) {
            flow[i].push_back({});
            for (qulonglong time = start_time; time <= end_time; time += time_div) {
                flow[i][j] << 0;
            }
        }
    }
    flow_per_hour = flow;
}

void TaskFlowAnalysis::process_flow_data(const TaskFlowAnalysis::FlowResult &flow_) {
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
}

QVector<unsigned long long> TaskFlowAnalysis::get_flow_time() {
    QMutexLocker l(&_data_mutex);
    return flow_time;
}

QVector<QVector<QVector<unsigned long long>>> TaskFlowAnalysis::get_flow_result() {
    QMutexLocker l(&_data_mutex);
    return flow;
}

QVector<QVector<QVector<unsigned long long>>> TaskFlowAnalysis::get_flow_per_hour_result() {
    QMutexLocker l(&_data_mutex);
    return flow_per_hour;
}

void TaskFlowAnalysis::postprocess_flow_data() {
    const int window_size = 3600 / time_div;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            qulonglong sum = 0;
            int _size = flow[i][j].size();
            for (int tb = 0; tb < _size; tb++) {
                int sub = tb - window_size;
                sum += flow[i][j][tb];
                if (sub >= 0) sum -= flow[i][j][sub];
                flow_per_hour[i][j][tb] = sum;
            }
        }
    }
}
