//
// Created by Alex Chi on 2019/12/03.
//

#ifndef METRO_TASKQUERYENTRYEXIT_H
#define METRO_TASKQUERYENTRYEXIT_H

#include "Task.h"
#include <QObject>

class TaskQueryEntryExit : public Task {
    Q_OBJECT

private:
    long long start_time;
    long long end_time;
    long long time_div;
    long long station_id;
    QString line_id;
public:
    TaskQueryEntryExit(QObject *parent = nullptr);

    void run() override;

    QString name() override;

    QList<Task*> dependencies() override;

    QString display_name() override;

    ~TaskQueryEntryExit() override;

    struct EntryExitResult {
        unsigned long long timestamp;
        unsigned long long status;
        unsigned long long count;
    };

    QMutex _data_mutex;
    QList<EntryExitResult> data;

signals:
    void result();

protected:
    bool parse_args() override;
};


#endif //METRO_TASKQUERYENTRYEXIT_H
