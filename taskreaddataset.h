#ifndef TASKREADDATASET_H
#define TASKREADDATASET_H

#include "task.h"

class TaskReadDataset : public Task
{
public:
    TaskReadDataset(QObject *parent = nullptr);
    bool journal() override;
    void run() override;
    QString name() override;
    ~TaskReadDataset() override;
};

#endif // TASKREADDATASET_H
