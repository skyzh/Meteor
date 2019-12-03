#include "taskreaddataset.h"
#include "db.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QString>

TaskReadDataset::TaskReadDataset(QObject *parent) : Task(parent) {
}

bool TaskReadDataset::journal() {
    return true;
}

QString TaskReadDataset::name() {
    return "Read Dataset";
}

void TaskReadDataset::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit message(QString("Failed to open SQL connection: %1").arg(db.lastError().text()));
        emit success(false);
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
            "create table dataset (time text, lineID text, stationID int, deviceID int, status int, userID text, payType int)";
    if (!q.exec(DATASET_INIT)) {
        emit message(QString("SQL Error: %1").arg(q.lastError().text()));
        emit success(false);
        return;
    }

    int cnt = 0;

    foreach(QString filename, datasets) {
        emit message(QString("Importing %1").arg(filename));
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
        int col_n = header.split(",").length();
        for (int i = 1; i < col_n; i++) value_placeholder += ",?";

        // [5] Insert into database
        auto sql_statement = QString("insert into dataset (%1) values (%2)").arg(header).arg(value_placeholder);

        q.prepare(sql_statement);
        db.transaction();
        while (!in.atEnd()) {
            auto row = in.readLine().split(",");
            for (int i = 0; i < col_n; i++) q.bindValue(i, row[i]);
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
