//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_TASKGETMAPPING_H
#define METRO_TASKGETMAPPING_H

#include "Task.h"
#include <QStringList>
#include <QVector>

class TaskGetMapping : public Task {
Q_OBJECT
public:
    TaskGetMapping(QObject *parent = nullptr);

    bool journal() override;

    void run() override;

    QString name() override;

    QString display_name() override;

    ~TaskGetMapping() override;

    struct Mapping {
        qulonglong stationID;
        QString lineID;
        QString name;
        QString chnName;
    };

    QVector<Mapping> get_data();

    QMap<QString, QList<qulonglong>> get_metros();


private:
    QVector<Mapping> data;
    QMap<QString, QList<qulonglong>> metros;

    QMutex _data_mutex;

signals:

    void result();
};


#endif //METRO_TASKGETMAPPING_H
