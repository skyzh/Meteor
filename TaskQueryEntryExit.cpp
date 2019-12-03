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

TaskQueryEntryExit::TaskQueryEntryExit(QObject *parent) : TaskQuery(parent) {

}

void TaskQueryEntryExit::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit message(QString("Failed to open SQL connection: %1").arg(db.lastError().text()));
        emit success(false);
        return;
    }

    QSqlQuery q(db);

    auto start_timestamp = get_arg(0).toUInt(), end_timestamp = get_arg(1).toUInt();

    qDebug() << get_timestamp_str(start_timestamp) << " " << get_timestamp_str(end_timestamp);

    emit success(true);
}

QString TaskQueryEntryExit::name() {
    return "query_entry_exit";
}

QString TaskQueryEntryExit::display_name() {
    return "Query Entry / Exit";
}

TaskQueryEntryExit::~TaskQueryEntryExit() {

}
