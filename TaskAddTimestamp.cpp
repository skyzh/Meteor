//
// Created by Alex Chi on 2019/12/03.
//

#include "TaskAddTimestamp.h"
#include "db.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QDateTime>

TaskAddTimestamp::TaskAddTimestamp(QObject *parent) : Task(parent) {

}

bool TaskAddTimestamp::journal() {
    return true;
}

void TaskAddTimestamp::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit message(QString("Failed to open SQL connection: %1").arg(db.lastError().text()));
        emit success(false);
        return;
    }

    QSqlQuery q(db);
    QSqlQuery u(db);

    if (!q.exec("select count(*) from dataset")) {
        emit_sql_error("query error when counting", q);
        return;
    }

    q.next();
    int N = q.value(0).toInt();
    const int STEP = 100000;

    q.prepare("select * from dataset where ? <= id and id < ?");
    for (int i = 0; i < N; i += STEP) {
        q.bindValue(0, i);
        q.bindValue(1, STEP + i);
        if (!q.exec()) {
            emit_sql_error("query error when selecting", q);
            return;
        }
        db.transaction();
        u.prepare("update dataset set timestamp=? where id=?");
        while (q.next()) {
            unsigned int id = q.value(0).toUInt();
            QString datetime = q.value(1).toString();
            QByteArray _datatime = datetime.toLatin1();
            /* It's much faster to use c-style convert */
            tm _tm;
            time_t epoch;
            if (strptime(_datatime.data(), "%Y-%m-%d %H:%M:%S", &_tm) != NULL)
                epoch = mktime(&_tm);
            u.bindValue(0, qlonglong(epoch));
            u.bindValue(1, id);
            if (!u.exec()) {
                emit_sql_error("updating error", u);
                return;
            }
        }
        if (!db.commit()) {
            emit message(QString("Commit error: %1").arg(db.lastError().text()));
            emit success(false);
            return;
        }
        emit progress(double(i) / N);
        if (_cancel) break;
    }


    emit success(true);
}

QString TaskAddTimestamp::name() {
    return "add_timestamp";
}

QString TaskAddTimestamp::display_name() {
    return "Converting Date and Time";
}

TaskAddTimestamp::~TaskAddTimestamp() {

}

QStringList TaskAddTimestamp::dependencies() {
    return {"read_dataset"};
}
