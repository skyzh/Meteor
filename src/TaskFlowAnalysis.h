//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_TASKFLOWANALYSIS_H
#define METRO_TASKFLOWANALYSIS_H

#include "Task.h"
#include <QVector>

struct FlowResult {
    unsigned long long enter_time;
    unsigned long long exit_time;
    unsigned long long enter_station;
    unsigned long long exit_station;
    QString userID;
};

class TaskFlowAnalysis : public Task {
Q_OBJECT
public:
    TaskFlowAnalysis(QObject *parent = nullptr);

    void run() override;

    bool journal() override;

    QString name() override;

    QList<Task *> dependencies() override;

    QString display_name() override;

    ~TaskFlowAnalysis() override;

private:
    unsigned long long start_time;
    unsigned long long end_time;
    const unsigned long long time_div = 60;

    QVector<FlowResult> data;
    QVector<QVector<QVector<unsigned long long>>> flow;
    QVector<QVector<QList<qulonglong>>> route_cache;
    QVector<unsigned long long> flow_time;

    void init_flow_data();

    void process_flow_data(const FlowResult &flow_);

    // void postprocess_flow_data();

    unsigned N;

protected:
    bool parse_args() override;
};


#endif //METRO_TASKFLOWANALYSIS_H
