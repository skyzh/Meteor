//
// Created by Alex Chi on 2019/12/03.
//

#ifndef METRO_TASKADDTIMESTAMP_H
#define METRO_TASKADDTIMESTAMP_H

#include "Task.h"

class TaskAddTimestamp : public Task {
public:
    TaskAddTimestamp(QObject *parent = nullptr);

    bool journal() override;

    void run() override;

    QString name() override;

    QString display_name() override;

    ~TaskAddTimestamp() override;
};


#endif //METRO_TASKADDTIMESTAMP_H
