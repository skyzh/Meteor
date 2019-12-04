//
// Created by Alex Chi on 2019/12/03.
//

#include "TaskQueryEntryExit.h"
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
                          "group by time_div, status";
    q.prepare(QUERY);
    q.bindValue(":time_div", time_div);
    q.bindValue(":start_time", start_time);
    q.bindValue(":end_time", end_time);
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
}

QString TaskQueryEntryExit::name() {
    return "query_entry_exit";
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
    return true;
}
