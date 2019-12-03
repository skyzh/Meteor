#ifndef TASKDELAY_H
#define TASKDELAY_H

#include "Task.h"

#include <QThread>

class TaskDelay : public Task {
public:
    TaskDelay(QObject *parent = nullptr);

    bool journal() override;

    void run() override;

    QString name() override;

    QString display_name() override;

    ~TaskDelay() override;
};

#endif // TASKDELAY_H
