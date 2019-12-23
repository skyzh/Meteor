//
// Created by Alex Chi on 2019/12/08.
//

#ifndef METRO_TASKQUERYSMARTTRAVEL_H
#define METRO_TASKQUERYSMARTTRAVEL_H

#include "Task.h"
#include "SmartTravelModel.hpp"

#include <QVector>
#include <QList>

class TaskQuerySmartTravel : public Task {
Q_OBJECT

public:
    TaskQuerySmartTravel(QObject *parent = nullptr);

    bool journal() override;

    QList<Task *> dependencies() override;

    QString name() override;

    QString display_name() override;

    QList<qulonglong> route_stations;

    QVector<double> get_estimated_time();

protected:
    void run() override;

    ~TaskQuerySmartTravel() override;

    bool parse_args() override;

signals:

    void result();

private:
    unsigned long long departure_time;
    unsigned long long start_time;
    const unsigned long long time_div = 60;

    void preprocess_distance_mapping();

    QVector<QVector<int>> distance;

    QVector<double> estimated_time;

    double travel_time;
};


#endif //METRO_TASKQUERYSMARTTRAVEL_H
