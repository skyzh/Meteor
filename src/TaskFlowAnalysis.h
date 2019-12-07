//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_TASKFLOWANALYSIS_H
#define METRO_TASKFLOWANALYSIS_H

#include "Task.h"
#include <QVector>

class TaskFlowAnalysis : public Task {
Q_OBJECT
public:
    TaskFlowAnalysis(QObject *parent = nullptr);

    void run() override;

    QString name() override;

    QList<Task *> dependencies() override;

    QString display_name() override;

    ~TaskFlowAnalysis() override;

    struct FlowResult {
        unsigned long long enter_time;
        unsigned long long exit_time;
        unsigned long long enter_station;
        unsigned long long exit_station;
        QString userID;
    };

    QVector<QVector<QVector<unsigned long long>>> get_flow_result();

    QVector<QVector<QVector<unsigned long long>>> get_flow_per_hour_result();

    QVector<unsigned long long> get_flow_time();

private:
    unsigned long long start_time;
    unsigned long long end_time;
    unsigned long long time_div;

    QMutex _data_mutex;
    QVector<FlowResult> data;
    QVector<QVector<QVector<unsigned long long>>> flow;
    QVector<QVector<QVector<unsigned long long>>> flow_per_hour;
    QVector<QVector<QList<qulonglong>>> route_cache;
    QVector<unsigned long long> flow_time;

    void init_flow_data();

    void process_flow_data(const FlowResult &flow_);

    void postprocess_flow_data();

    unsigned N;

signals:

    void result();

protected:
    bool parse_args() override;
};


#endif //METRO_TASKFLOWANALYSIS_H
