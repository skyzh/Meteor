//
// Created by Alex Chi on 2019/12/06.
//

#include "TaskGetMapping.h"
#include "ConfigManager.h"
#include <QTextStream>
#include <QFile>

bool TaskGetMapping::journal() {
    return false;
}

void TaskGetMapping::run() {
    {
        QMutexLocker l(&_data_mutex);
        const QString path = QString("%1/adjacency_adjacency/station_line.csv").arg(
                ConfigManager::instance()->get("DATASET_PATH").toString());
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            emit message(QString("Failed to open file %1").arg(path));
            emit success(false);
            return;
        }
        QTextStream in(&file);

        bool first_line = true;

        int id = 0;

        while (!in.atEnd()) {
            QStringList list = in.readLine().split(",");
            if (first_line) {
                first_line = false;
                continue;
            }
            if (list.first().toInt() != id++) {
                emit message("Invalid Mapping");
                emit success(false);
                return;
            }
            data << Mapping {
                list[0].toULongLong(), list[1], list[2], list[3]
            };
        }
    }
    emit result();
    emit success(true);
}

QString TaskGetMapping::name() {
    return "get_mapping";
}

QString TaskGetMapping::display_name() {
    return "Get Station Name";
}

QList<TaskGetMapping::Mapping> TaskGetMapping::get_data() {
    QMutexLocker l(&_data_mutex);
    return data;
}

TaskGetMapping::~TaskGetMapping() {

}

TaskGetMapping::TaskGetMapping(QObject *parent) : Task(parent) {

}

