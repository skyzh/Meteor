//
// Created by Alex Chi on 2019/12/07.
//

#include "TaskQueryFlow.h"
#include "TaskFlowAnalysis.h"
#include "TaskPlanRoute.h"
#include "db.h"

#include <QDebug>

QVector<unsigned long long> TaskQueryFlow::get_flow_time() {
    QMutexLocker l(&_data_mutex);
    return flow_time;
}

QVector<QVector<QVector<unsigned long long>>> TaskQueryFlow::get_flow_result() {
    QMutexLocker l(&_data_mutex);
    return flow;
}

QVector<QVector<QVector<unsigned long long>>> TaskQueryFlow::get_flow_per_hour_result() {
    QMutexLocker l(&_data_mutex);
    return flow_per_hour;
}

void TaskQueryFlow::postprocess_flow_data() {
    const int window_length = 300;
    const int window_size = window_length / time_div;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            qulonglong sum = 0;
            int _size = flow[i][j].size();
            for (int tb = 0; tb < _size; tb++) {
                int sub = tb - window_size;
                sum += flow[i][j][tb];
                if (sub >= 0) sum -= flow[i][j][sub];
                flow_per_hour[i][j][tb] = sum * (3600 / window_length);
            }
        }
    }
}

bool TaskQueryFlow::journal() {
    return false;
}

QList<Task *> TaskQueryFlow::dependencies() {
    auto task = new TaskFlowAnalysis(this);
    task->args({start_time, end_time});
    return {task};
}

QString TaskQueryFlow::name() {
    return "query_flow";
}

QString TaskQueryFlow::display_name() {
    return "Estimating Segment Flow";
}

void TaskQueryFlow::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Open SQL connection", db);
        return;
    }

    QSqlQuery q(db);

    QMap<QString, FlowResult> mapping;

    // [1] Initialize Flow Vector

    init_flow_data();
    emit progress(0);

    // [2] Fetch Data from Database
    q.prepare("select * from flow where start_time=:start_time");
    q.bindValue(":start_time", start_time);

    if (!q.exec()) {
        emit_sql_error("Error when querying", q);
        return;
    }

    while (q.next()) {
        auto &flow_ = flow[q.value(1).toLongLong()][q.value(2).toLongLong()][q.value(3).toLongLong()];
        auto result = q.value(4).toLongLong();
        if (flow_ != 0) {
            if (flow_ != result) {
                emit message("Inconsistent flow data");
                emit success(false);
                return;
            } else {
                emit message("Duplicate flow data");
                emit success(false);
                return;
            }
        } else {
            flow_ = result;
        }
    }

    if (_cancel) {
        emit success(false);
        return;
    }

    // [3] Obtain Hourly Data
    postprocess_flow_data();

    if (!_cancel) {
        emit result();
        emit success(true);
    } else {
        emit success(false);
    }

    db.close();
}

bool TaskQueryFlow::parse_args() {
    {
        QDateTime timestamp;
        timestamp.setTime_t(get_arg(0).toUInt());
        timestamp.setTime(QTime(4, 0));
        start_time = timestamp.toSecsSinceEpoch();
    }
    {
        QDateTime timestamp;
        timestamp.setTime_t(get_arg(1).toUInt());
        timestamp.setTime(QTime(4, 0));
        end_time = timestamp.toSecsSinceEpoch();
    }
    return true;
}

TaskQueryFlow::TaskQueryFlow(QObject *parent) : Task(parent) {

}

void TaskQueryFlow::init_flow_data() {
    auto mat = TaskPlanRoute::get_route_mapping();
    N = mat.size();
    TaskBaseFlowAnalysis::init_flow_time(flow_time, start_time, end_time, time_div);
    TaskBaseFlowAnalysis::init_flow_matrix(flow, N, start_time, end_time, time_div);
    TaskBaseFlowAnalysis::init_flow_matrix(flow_per_hour, N, start_time, end_time, time_div);
}
