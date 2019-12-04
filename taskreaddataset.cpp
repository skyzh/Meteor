#include "TaskReadDataset.h"
#include "db.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QDateTime>

TaskReadDataset::TaskReadDataset(QObject *parent) : Task(parent) {
}

bool TaskReadDataset::journal() {
    return true;
}

QString TaskReadDataset::name() {
    return "read_dataset";
}

QString TaskReadDataset::display_name() {
    return "Building database";
}

void TaskReadDataset::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Failed to open SQL connection", db);
        return;
    }

    QSqlQuery q(db);
    // [0] Drop table
    emit message(QString("Purging database..."));
    if (!q.exec("drop table if exists dataset")) {
        emit message(QString("SQL Error: %1").arg(q.lastError().text()));
        emit success(false);
        return;
    }

    // [1] Listing all files under dataset path
    const QString DATASET_PATH = "/Users/skyzh/Work/Qt/dataset_CS241/dataset/";
    QDir directory(DATASET_PATH);
    QStringList datasets = directory.entryList(QStringList() << "*.csv" << "*.CSV", QDir::Files);

    // [2] Create table
    const QString DATASET_INIT = \
            "create table dataset (id int primary key, time int, lineID text, stationID int, deviceID int, status int, userID text, payType int)";
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

    int cnt = 0;
    int id = 0;
    tm _tm;
    time_t epoch;

    foreach(QString filename, datasets) {
        emit message(QString("processing %1").arg(filename));
        emit progress(double(cnt++) / datasets.length());

        // [3] Try open file
        QFile file(QString("%1%2").arg(DATASET_PATH).arg(filename));
        if (!file.open(QIODevice::ReadOnly)) {
            emit message(QString("Failed to open file %1").arg(filename));
            emit success(false);
            return;
        }

        // [4] Load and parse file
        QTextStream in(&file);
        auto header = in.readLine();
        QString value_placeholder = "?";
        QStringList _header = header.split(",");
        int col_n = _header.length();
        for (int i = 1; i < col_n; i++) value_placeholder += ",?";

        int time_col = _header.indexOf("time");

        // [5] Insert into database
        auto sql_statement = QString("insert into dataset (%1,id) values (%2,?)").arg(header).arg(value_placeholder);

        q.prepare(sql_statement);
        db.transaction();
        while (!in.atEnd()) {
            auto row = in.readLine().split(",");
            for (int i = 0; i < col_n; i++) {
                if (i == time_col) {
                    QByteArray _datetime = row[i].toLatin1();
                    if (strptime(_datetime.data(), "%Y-%m-%d %H:%M:%S", &_tm) != NULL)
                        epoch = mktime(&_tm);
                    q.bindValue(i, (qulonglong) epoch);
                } else
                    q.bindValue(i, row[i]);
            }
            q.bindValue(col_n, id++);
            if (!q.exec()) {
                emit message(QString("SQL Error: %1").arg(q.lastError().text()));
                emit success(false);
                return;
            }
        }
        db.commit();

        if (!q.exec("select count(*) from dataset")) {
            emit message(QString("SQL Error: %1").arg(q.lastError().text()));
            emit success(false);
            return;
        }

        q.next();
        // qDebug() << QString("%1 records in database").arg(q.value(0).toInt());

        if (_cancel) break;
    }

    emit success(true);
}

TaskReadDataset::~TaskReadDataset() {}
