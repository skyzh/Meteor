//
// Created by Alex Chi on 2019/12/03.
//

#include "TaskQueryEntryExit.h"
#include "TaskReadDataset.h"
#include "db.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDateTime>
#include <QDebug>

QString get_timestamp_str(unsigned int tsp) {
    QDateTime timestamp;
    timestamp.setTime_t(tsp);
    return timestamp.toString(Qt::SystemLocaleShortDate);
}

TaskQueryEntryExit::TaskQueryEntryExit(QObject *parent) : Task(parent) {

}

void TaskQueryEntryExit::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Open SQL connection", db);
        return;
    }

    QSqlQuery q(db);

    QString QUERY = "select (time / :time_div) as time_div, "
                    "status, "
                    "count(*) from dataset "
                    "where "
                    ":start_time <= time "
                    "and time < :end_time "
                    "and %1 "
                    "and %2 "
                    "group by time_div, status";
    if (station_id < 0)
        QUERY = QUERY.arg("1");
    else QUERY = QUERY.arg("stationID = :station_id");

    if (line_id == "")
        QUERY = QUERY.arg("1");
    else QUERY = QUERY.arg("lineID = :line_id");

    q.prepare(QUERY);

    q.bindValue(":time_div", time_div);
    q.bindValue(":start_time", start_time);
    q.bindValue(":end_time", end_time);
    if (line_id != "")
        q.bindValue(":line_id", line_id);
    if (station_id >= 0)
        q.bindValue(":station_id", station_id);
    if (!q.exec()) {
        emit_sql_error("Query Error", q);
        return;
    }

    int cnt = 0;

    {
        QMutexLocker l(&_data_mutex);
        data.clear();
        auto _stt = start_time / time_div;
        auto _ett = end_time / time_div;

        auto lst_time_0 = _stt;
        auto lst_time_1 = _stt;
        while (q.next()) {
            auto time_block = q.value(0).toULongLong();
            auto status = q.value(1).toULongLong();
            if (status == 0) {
                for (unsigned long long i = lst_time_0; i < time_block; i++) {
                    data << EntryExitResult{i * time_div, 0, 0};
                }
                lst_time_0 = time_block + 1;
            }
            if (status == 1) {
                for (unsigned long long i = lst_time_1; i < time_block; i++) {
                    data << EntryExitResult{i * time_div, 1, 0};
                }
                lst_time_1 = time_block + 1;
            }
            data << EntryExitResult{
                    time_block * time_div,
                    status,
                    q.value(2).toULongLong()
            };
        }
    }

    emit result();

    emit success(true);

    db.close();

}

QString TaskQueryEntryExit::name() {
    return QString("query_entry_exit:%1:%2:%3")
            .arg(time_div)
            .arg(start_time)
            .arg(end_time);
}

QString TaskQueryEntryExit::display_name() {
    return QString("Query Entry/Exit");
}

TaskQueryEntryExit::~TaskQueryEntryExit() {

}

bool TaskQueryEntryExit::parse_args() {
    time_div = get_arg(0).toUInt();
    start_time = get_arg(1).toUInt();
    end_time = get_arg(2).toUInt();
    line_id = get_arg(3).toString();
    station_id = get_arg(4).toUInt();
    if (get_arg(3).toString() == "All") line_id = "";
    if (get_arg(4).toString() == "All") station_id = -1;
    return true;
}

QList<Task *> TaskQueryEntryExit::dependencies() {
    QList<Task *> dependencies;
    for (long long time = start_time; time < end_time; time += 86400) {
        QDateTime timestamp;
        timestamp.setTime_t(time);
        QString date = timestamp.toString("yyyy-MM-dd");
        TaskReadDataset *task = new TaskReadDataset(parent());
        task->args({date});
        dependencies << task;
    }
    return dependencies;
}
