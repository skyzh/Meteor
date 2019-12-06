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

    const QString QUERY = "select (time / :time_div) as time_div, "
                          "status, "
                          "count(*) from dataset "
                          "where "
                          ":start_time <= time "
                          "and time < :end_time "
                          "%1 "
                          "group by time_div, status";
    if (query_all) q.prepare(QUERY.arg(""));
    else q.prepare(QUERY.arg("and stationID = :station_id and lineID = :line_id"));
    q.bindValue(":time_div", time_div);
    q.bindValue(":start_time", start_time);
    q.bindValue(":end_time", end_time);
    if (!query_all) {
        q.bindValue(":station_id", station_id);
        q.bindValue(":line_id", line_id);
    }
    qDebug() << query_all << " " << station_id << " " << line_id;
    if (!q.exec()) {
        emit_sql_error("Query Error", q);
        return;
    }

    int cnt = 0;

    {
        QMutexLocker l(&_data_mutex);
        data.clear();
        while (q.next()) {
            data.push_back(EntryExitResult{
                    q.value(0).toULongLong() * time_div,
                    q.value(1).toULongLong(),
                    q.value(2).toULongLong()
            });
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
    return QString("Query Station %2")
        .arg(get_timestamp_str(start_time));
}

TaskQueryEntryExit::~TaskQueryEntryExit() {

}

bool TaskQueryEntryExit::parse_args() {
    time_div = get_arg(0).toUInt();
    start_time = get_arg(1).toUInt();
    end_time = get_arg(2).toUInt();
    if (get_arg(3).toString() == "all") query_all = true;
    else {
        line_id = get_arg(3).toString();
        station_id = get_arg(4).toUInt();
    }
    return true;
}

QList<Task*> TaskQueryEntryExit::dependencies() {
    QList<Task*> dependencies;
    for (long long time = start_time; time < end_time; time += 86400) {
        QDateTime timestamp;
        timestamp.setTime_t(time);
        QString date = timestamp.toString("yyyy-MM-dd");
        TaskReadDataset *task = new TaskReadDataset(parent());
        task->args({ date });
        dependencies.push_back(task);
    }
    return dependencies;
}
