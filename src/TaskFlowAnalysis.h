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
    const unsigned long long time_div = 60;
};

inline bool TaskFlowAnalysis::process_flow_data(const FlowResult &flow_) {
    qulonglong enter_time_block = (flow_.enter_time - start_time) / time_div,
            exit_time_block = (flow_.exit_time - start_time) / time_div;
    qulonglong time_block_cnt = exit_time_block - enter_time_block;
    qulonglong lst_time_block = enter_time_block;
    auto &route = route_cache[flow_.enter_station][flow_.exit_station];
    int route_size = route.size() - 1;
    for (int i = 0; i < route_size; i++) {
        qulonglong current_time_block = double(i) / route_size * time_block_cnt + enter_time_block;
        auto route_enter = route[i];
        auto route_exit = route[i + 1];
        for (qulonglong tb = lst_time_block; tb <= current_time_block; tb++) {
            ++flow[route_enter][route_exit][tb];
        }
        lst_time_block = current_time_block;
    }
    return true;
}

#endif //METRO_TASKFLOWANALYSIS_H
