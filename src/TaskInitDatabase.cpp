//
// Created by Alex Chi on 2019/12/06.
//

#include "TaskInitDatabase.h"
#include "db.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

bool TaskInitDatabase::journal() {
    return true;
}

void TaskInitDatabase::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Failed to open SQL connection", db);
        return;
    }

    QSqlQuery q(db);

    emit message(QString("Purging database..."));
    if (!q.exec("drop table if exists dataset")) {
        emit_sql_error("SQL Error", q);
        return;
    }

    if (!q.exec("drop table if exists flow")) {
        emit_sql_error("SQL Error", q);
        return;
    }

    // [2] Create table
    const QString DATASET_INIT = \
            "create table dataset (time int, lineID text, stationID int, deviceID int, status int, userID text, payType int)";

    if (!q.exec(DATASET_INIT)) {
        emit_sql_error("SQL Error", q);
        return;
    }

    if (!q.exec("create index time on dataset (time ASC)")) {
        emit_sql_error("SQL Error", q);
        return;
    }

    if (!q.exec("create index stationID_line on dataset (stationID ASC, lineID ASC)")) {
        emit_sql_error("SQL Error", q);
        return;
    }

    // [3] Create flow analysis table

    const QString FLOW_INIT = \
            "create table flow (start_time int, enter_station_id int, exit_station_id int, time_block int, flow int)";

    if (!q.exec(FLOW_INIT)) {
        emit_sql_error("SQL Error", q);
        return;
    }

    if (!q.exec("create index flow_query on flow (start_time ASC)")) {
        emit_sql_error("SQL Error", q);
        return;
    }

    if (!q.exec("create index flow_query_detail on flow (start_time ASC, enter_station_id ASC, exit_station_id ASC, time_block ASC)")) {
        emit_sql_error("SQL Error", q);
        return;
    }

    db.close();

    emit success(true);
}

QString TaskInitDatabase::name() {
    return "init_db_dataset";
}

QString TaskInitDatabase::display_name() {
    return "Initialize Database";
}

TaskInitDatabase::~TaskInitDatabase() {

}

TaskInitDatabase::TaskInitDatabase(QObject *parent) : Task(parent) {

}
