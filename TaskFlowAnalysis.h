//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_TASKFLOWANALYSIS_H
#define METRO_TASKFLOWANALYSIS_H

#include "Task.h"

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

    QList<FlowResult> get_data();

private:
    long long start_time;
    long long end_time;

    QMutex _data_mutex;
    QList<FlowResult> data;

signals:
    void result();

protected:
    bool parse_args() override;
};


#endif //METRO_TASKFLOWANALYSIS_H
