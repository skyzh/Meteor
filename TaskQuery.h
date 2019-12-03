#ifndef TASKQUERY_H
#define TASKQUERY_H

#include "Task.h"

class TaskQuery : public Task {
signals:
    void result(QObject result);

public:
    TaskQuery(QObject *parent = nullptr);

    bool journal() override;

    virtual QString name() override;
};

#endif // TASKQUERY_H
