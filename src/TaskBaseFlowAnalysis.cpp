//
// Created by Alex Chi on 2019/12/08.
//

#include "TaskBaseFlowAnalysis.h"


#include "db.h"
#include "TaskFlowAnalysis.h"
#include "TaskReadDataset.h"
#include "TaskPlanRoute.h"
#include "TaskInitDatabase.h"
#include "utils.h"

#include <QDateTime>
#include <QDebug>

TaskBaseFlowAnalysis::TaskBaseFlowAnalysis(QObject *parent) : Task(parent) {

}

void TaskBaseFlowAnalysis::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Open SQL connection", db);
        return;
    }

    QSqlQuery q(db);

    qulonglong failed_attempts = 0;
    const qulonglong TIME_STEP = 3600;

    QMap<QString, FlowResult> mapping;

    // [1] Initialize Flow Vector and Database
    if (!init_flow_data(q)) return;
    emit progress(0);

    QString QUERY = "select * from dataset "
                    "where :start_time <= time and time < :end_time "
                    "order by time asc, status desc";

    q.prepare(QUERY);

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

                    // [3] Process Flow Record
                    if (!process_flow_data(flow_)) return;
                }
            }
        }
        if (_cancel) break;
        emit progress(double(time - start_time) / (end_time - start_time));
    }

    // [4] Commit to Database

    db.transaction();

    if (!commit_to_database(q)) return;

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

QString TaskBaseFlowAnalysis::name() {
    QDateTime timestamp;
    timestamp.setTime_t(start_time);
    return QString("%1")
            .arg(timestamp.toString("yyyy-MM-dd"));
}

QList<Task *> TaskBaseFlowAnalysis::dependencies() {
    auto deps = TaskReadDataset::from_time_range(start_time, end_time, this, true);
    deps.push_front(new TaskInitDatabase);
    return deps;
}

QString TaskBaseFlowAnalysis::display_name() {
    QDateTime timestamp;
    timestamp.setTime_t(start_time);
    return QString("%1")
            .arg(timestamp.toString("yyyy-MM-dd"));
}

TaskBaseFlowAnalysis::~TaskBaseFlowAnalysis() {

}

bool TaskBaseFlowAnalysis::parse_args() {
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

bool TaskBaseFlowAnalysis::init_flow_data(QSqlQuery &q) {
    return false;
}

bool TaskBaseFlowAnalysis::process_flow_data(const FlowResult &flow_) {
    return false;
}

bool TaskBaseFlowAnalysis::journal() {
    return false;
}

void TaskBaseFlowAnalysis::init_flow_time(QVector<unsigned long long> &flow_time, unsigned long long start_time,
                                          unsigned long long end_time, unsigned long long time_div) {
    flow_time.clear();
    for (qulonglong time = start_time; time <= end_time; time += time_div) {
        flow_time << time;
    }
}

void TaskBaseFlowAnalysis::init_flow_matrix(QVector<QVector<QVector<unsigned long long>>> &flow, unsigned N,
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

bool TaskBaseFlowAnalysis::commit_to_database(QSqlQuery &q) {
    return false;
}
