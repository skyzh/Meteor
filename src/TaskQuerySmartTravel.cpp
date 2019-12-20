//
// Created by Alex Chi on 2019/12/08.
//

#include "TaskQuerySmartTravel.h"
#include "TaskSmartTravel.h"

bool TaskQuerySmartTravel::journal() {
    return false;
}

QList<Task *> TaskQuerySmartTravel::dependencies() {
    auto task = new TaskSmartTravel(this);
    QDateTime timestamp;
    timestamp.setTime_t(departure_time);
    timestamp.setTime(QTime(4, 0));
    auto start_time = timestamp.toSecsSinceEpoch();
    task->args({start_time, start_time + 86400});
    return {task};
}

QString TaskQuerySmartTravel::name() {
    return "query_smart_travel";
}

QString TaskQuerySmartTravel::display_name() {
    return "Estimating Travel Time";
}

void TaskQuerySmartTravel::run() {
    Task::run();
}

TaskQuerySmartTravel::~TaskQuerySmartTravel() {

}

bool TaskQuerySmartTravel::parse_args() {
    departure_time = get_arg(0).toUInt();
    return true;
}

TaskQuerySmartTravel::TaskQuerySmartTravel(QObject *parent) : Task(parent) {

}
