//
// Created by Alex Chi on 2019/12/06.
//

#include "db.h"
#include "TaskFlowAnalysis.h"
#include "TaskReadDataset.h"
#include "TaskPlanRoute.h"
#include "TaskInitDatabase.h"
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

    qulonglong failed_attempts = 0;
    const qulonglong TIME_STEP = 3600;

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

    // [4] Commit to Database
    emit message("Commit to database");

    db.transaction();

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
                        return;
                    }
                }
            }
        }
    }

    if (_cancel) {
        db.rollback();
    } else {
        if (!db.commit()) {
            emit_db_error("Error when commit", db);
            return;
        }
        emit success(true);
    }

    db.close();
}

QString TaskFlowAnalysis::name() {
    QDateTime timestamp;
    timestamp.setTime_t(start_time);
    return QString("flow_analysis_%1")
            .arg(timestamp.toString("yyyy-MM-dd"));
}

QList<Task *> TaskFlowAnalysis::dependencies() {
    auto deps = TaskReadDataset::from_time_range(start_time, end_time, this, true);
    deps.push_front(new TaskInitDatabase);
    return deps;
}

QString TaskFlowAnalysis::display_name() {
    QDateTime timestamp;
    timestamp.setTime_t(start_time);
    return QString("Flow Analysis %1")
            .arg(timestamp.toString("yyyy-MM-dd"));
}

TaskFlowAnalysis::~TaskFlowAnalysis() {

}

bool TaskFlowAnalysis::parse_args() {
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

    init_flow_time(flow_time, start_time, end_time, time_div);
    init_flow_matrix(flow, N, start_time, end_time, time_div);
}

void TaskFlowAnalysis::process_flow_data(const FlowResult &flow_) {
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

bool TaskFlowAnalysis::journal() {
    return true;
}

void TaskFlowAnalysis::init_flow_time(QVector<unsigned long long> &flow_time, unsigned long long start_time,
                                      unsigned long long end_time, unsigned long long time_div) {
    flow_time.clear();
    for (qulonglong time = start_time; time <= end_time; time += time_div) {
        flow_time << time;
    }
}

void TaskFlowAnalysis::init_flow_matrix(QVector<QVector<QVector<unsigned long long>>> &flow, unsigned N,
                                        unsigned long long start_time,
                                        unsigned long long end_time, unsigned long long time_div) {
    flow.clear();

    for (int i = 0; i < N; i++) {
        flow.push_back({});
        for (int j = 0; j < N; j++) {
            flow[i].push_back({});
            for (qulonglong time = start_time; time <= end_time; time += time_div) {
                flow[i][j] << 0;
            }
        }
    }
}
