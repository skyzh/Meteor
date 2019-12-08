//
// Created by Alex Chi on 2019/12/08.
//

#ifndef METRO_TASKBASEFLOWANALYSIS_H
#define METRO_TASKBASEFLOWANALYSIS_H


#include "Task.h"
#include <QVector>
#include <QSqlQuery>

struct FlowResult {
    unsigned long long enter_time;
    unsigned long long exit_time;
    unsigned long long enter_station;
    unsigned long long exit_station;
    QString userID;
};

class TaskBaseFlowAnalysis : public Task {
Q_OBJECT
public:
    TaskBaseFlowAnalysis(QObject *parent = nullptr);

    void run() override;

    virtual bool journal() override;

    QString name() override;

    QList<Task *> dependencies() override;

    QString display_name() override;

    ~TaskBaseFlowAnalysis() override;


    static void init_flow_time(QVector<unsigned long long> &flow_time, unsigned long long start_time,
                               unsigned long long end_time, unsigned long long time_div);

    static void init_flow_matrix(QVector<QVector<QVector<unsigned long long>>> &flow, unsigned N,
                                 unsigned long long start_time,
                                 unsigned long long end_time, unsigned long long time_div);

protected:
    unsigned long long start_time;
    unsigned long long end_time;
    const unsigned long long time_div = 60;

    virtual bool init_flow_data(QSqlQuery& q);

    virtual bool process_flow_data(const FlowResult &flow_);

    virtual bool commit_to_database(QSqlQuery& q);

    unsigned N;

    bool parse_args() override;
};


#endif //METRO_TASKBASEFLOWANALYSIS_H
