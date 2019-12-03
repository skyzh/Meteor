//
// Created by Alex Chi on 2019/12/03.
//

#ifndef METRO_TASKQUERYENTRYEXIT_H
#define METRO_TASKQUERYENTRYEXIT_H

#include "TaskQuery.h"

class TaskQueryEntryExit : public TaskQuery {
public:
    TaskQueryEntryExit(QObject *parent = nullptr);

    void run() override;

    QString name() override;

    QString display_name() override;

    ~TaskQueryEntryExit() override;
};


#endif //METRO_TASKQUERYENTRYEXIT_H
