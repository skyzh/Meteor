#ifndef TASKREADDATASET_H
#define TASKREADDATASET_H

#include "Task.h"

class TaskReadDataset : public Task {
private:
    QString at_date;

public:
    TaskReadDataset(QObject *parent = nullptr);

    bool journal() override;

    void run() override;

    QString name() override;

    QString display_name() override;

    ~TaskReadDataset() override;

    QList<Task *> dependencies() override;

    static QList<Task *> from_time_range(qulonglong start_time, qulonglong end_time, QObject *parent);

protected:
    bool parse_args() override;
};

#endif // TASKREADDATASET_H
