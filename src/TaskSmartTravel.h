//
// Created by Alex Chi on 2019/12/08.
//

#ifndef METRO_TASKSMARTTRAVEL_H
#define METRO_TASKSMARTTRAVEL_H

#include "TaskBaseFlowAnalysis.h"

class TaskSmartTravel : public TaskBaseFlowAnalysis {
public:
    TaskSmartTravel(QObject *parent);

    bool journal() override;

    QString name() override;

    QString display_name() override;

    ~TaskSmartTravel() override;

protected:
    bool init_flow_data(QSqlQuery& q) override;

    bool process_flow_data(const FlowResult &flow_) final;

    bool commit_to_database(QSqlQuery &q) override;

private:
    QVector<QVector<QVector<unsigned long long>>> flow_sum;
    QVector<QVector<QVector<unsigned long long>>> flow_n;
    QVector<unsigned long long> flow_time;
    const unsigned long long time_div = 60;
};

inline bool TaskSmartTravel::process_flow_data(const FlowResult &flow_) {
    qulonglong tb = (flow_.enter_time - start_time) / time_div;
    ++flow_n[flow_.enter_station][flow_.exit_station][tb];
    flow_sum[flow_.enter_station][flow_.exit_station][tb] += flow_.exit_time - flow_.enter_time;
    return true;
}

#endif //METRO_TASKSMARTTRAVEL_H
