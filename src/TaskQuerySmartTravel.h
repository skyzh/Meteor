//
// Created by Alex Chi on 2019/12/08.
//

#ifndef METRO_TASKQUERYSMARTTRAVEL_H
#define METRO_TASKQUERYSMARTTRAVEL_H

#include "Task.h"

class TaskQuerySmartTravel : public Task {
Q_OBJECT

public:
    TaskQuerySmartTravel(QObject *parent = nullptr);

    bool journal() override;

    QList<Task *> dependencies() override;

    QString name() override;

    QString display_name() override;

protected:
    void run() override;

    ~TaskQuerySmartTravel() override;

    bool parse_args() override;

signals:

    void result();

private:
    unsigned long long departure_time;
    const unsigned long long time_div = 60;
};


#endif //METRO_TASKQUERYSMARTTRAVEL_H
