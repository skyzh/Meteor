//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_TASKFLOWANALYSIS_H
#define METRO_TASKFLOWANALYSIS_H

#include "TaskBaseFlowAnalysis.h"

class TaskFlowAnalysis : public TaskBaseFlowAnalysis {
public:
    TaskFlowAnalysis(QObject *parent);

    bool journal() override;

    QString name() override;

    QString display_name() override;

    ~TaskFlowAnalysis() override;

protected:
    bool init_flow_data(QSqlQuery& q) override;

    bool process_flow_data(const FlowResult &flow_) override;

    bool commit_to_database(QSqlQuery &q) override;

private:
    QVector<QVector<QVector<unsigned long long>>> flow;
    QVector<QVector<QList<qulonglong>>> route_cache;
    QVector<unsigned long long> flow_time;
};

#endif //METRO_TASKFLOWANALYSIS_H
