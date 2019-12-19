#include "TaskReadDataset.h"
#include "TaskInitDatabase.h"
#include "ConfigManager.h"
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
    return QString("read_dataset_%1").arg(at_date);
}

QString TaskReadDataset::display_name() {
    return QString("Reading %1").arg(at_date);
}

void TaskReadDataset::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Failed to open SQL connection", db);
        return;
    }

    QSqlQuery q(db);

    // [0] Listing all files under dataset path
    const QString DATASET_PATH =
            QString("%1/dataset/").arg(ConfigManager::instance()->get("DATASET_PATH").toString());
    QDir directory(DATASET_PATH);
    QStringList _datasets = directory.entryList(QStringList() << "*.csv" << "*.CSV", QDir::Files);
    QStringList datasets;
    QString filename_prefix = QString("record_%1").arg(at_date);
    for (auto &&dataset : _datasets) {
        if (dataset.startsWith(filename_prefix))
            datasets << dataset;
    }

    int cnt = 0;
    int id = 0;
    tm _tm;
    time_t epoch;

    db.transaction();

    emit message("Processing files");

    for (auto &&filename : datasets) {
        emit progress(double(cnt++) / datasets.length());

        // [1] Try open file
        QFile file(QString("%1%2").arg(DATASET_PATH).arg(filename));
        if (!file.open(QIODevice::ReadOnly)) {
            emit message(QString("Failed to open file %1").arg(filename));
            emit success(false);
            return;
        }

        // [2] Load and parse file
        QTextStream in(&file);
        auto header = in.readLine();
        QString value_placeholder = "?";
        QStringList _header = header.split(",");
        int col_n = _header.length();
        for (int i = 1; i < col_n; i++) value_placeholder += ",?";

        int time_col = _header.indexOf("time");

        // [3] Insert into database
        auto sql_statement = QString("insert into dataset (%1) values (%2)").arg(header).arg(value_placeholder);

        q.prepare(sql_statement);

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
            if (!q.exec()) {
                emit_sql_error("SQL Error", q);
                return;
            }
        }
        if (_cancel) break;
    }

    if (_cancel) {
        db.rollback();
    } else {
        emit message("Committing changes");
        if (!db.commit()) {
            emit_db_error("Failed to commit", db);
            return;
        }
        emit success(true);
    }

    db.close();
}

TaskReadDataset::~TaskReadDataset() {}

bool TaskReadDataset::parse_args() {
    at_date = get_arg(0).toString();
    return true;
}

QList<Task *> TaskReadDataset::dependencies() {
    return {new TaskInitDatabase};
}

QList<Task *>
TaskReadDataset::from_time_range(qulonglong _start_time, qulonglong _end_time, QObject *parent, bool include_tail) {
    QDateTime start_timestamp;
    start_timestamp.setTime_t(_start_time);
    start_timestamp.setTime(QTime(0, 0, 0));
    unsigned long long start_time = start_timestamp.toSecsSinceEpoch();
    QDateTime end_timestamp;
    end_timestamp.setTime_t(include_tail ? _end_time : _end_time - 1);
    end_timestamp.setTime(QTime(0, 0, 0));
    unsigned long long end_time = end_timestamp.toSecsSinceEpoch();
    QList<Task *> dependencies;
    for (unsigned long long time = start_time; time <= end_time; time += 86400) {
        QDateTime timestamp;
        timestamp.setTime_t(time);
        QString date = timestamp.toString("yyyy-MM-dd");
        TaskReadDataset *task = new TaskReadDataset(parent);
        task->args({date});
        dependencies << task;
    }
    return dependencies;
}
