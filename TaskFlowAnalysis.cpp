//
// Created by Alex Chi on 2019/12/06.
//

#include "db.h"
#include "TaskFlowAnalysis.h"
#include "TaskReadDataset.h"

#include <QDateTime>
#include <QDebug>

QString get_timestamp_str(unsigned int tsp) {
    QDateTime timestamp;
    timestamp.setTime_t(tsp);
    return timestamp.toString(Qt::SystemLocaleShortDate);
}


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
        for (qulonglong time = start_time; time < end_time; time += TIME_STEP) {
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
                        FlowResult flow = mapping[userID];
                        mapping.remove(userID);
                        flow.exit_time = time;
                        flow.exit_station = stationID;
                        data << flow;
                    }
                }
            }
            if (_cancel) break;
            emit progress(double(time - start_time) / (end_time - start_time));
        }
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

QList<TaskFlowAnalysis::FlowResult> TaskFlowAnalysis::get_data() {
    return QList<TaskFlowAnalysis::FlowResult>();
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
    return true;
}
