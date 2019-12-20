//
// Created by Alex Chi on 2019/12/07.
//

#ifndef METRO_TASKQUERYFLOW_H
#define METRO_TASKQUERYFLOW_H

#include "Task.h"
#include <QVector>

class TaskQueryFlow : public Task {
Q_OBJECT

public:
    TaskQueryFlow(QObject *parent);

    bool journal() override;

    QList<Task *> dependencies() override;

    QString name() override;

    QString display_name() override;

protected:
    void run() override;

    bool parse_args() override;

public:
    QVector<unsigned long long> get_flow_time();

    QVector<QVector<QVector<unsigned long long>>> get_flow_result();

    QVector<QVector<QVector<unsigned long long>>> get_flow_per_hour_result();

signals:

    void result();

private:
    QVector<QVector<QVector<unsigned long long>>> flow;
    QVector<unsigned long long> flow_time;
    QVector<QVector<QVector<unsigned long long>>> flow_per_hour;
    QMutex _data_mutex;

    void init_flow_data();

    void postprocess_flow_data();

    unsigned long long start_time;
    unsigned long long end_time;
    const unsigned long long time_div = 60;
    unsigned long long N;
};


#endif //METRO_TASKQUERYFLOW_H
