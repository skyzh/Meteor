#include "Task.h"
#include "TaskReadDataset.h"
#include "TaskDelay.h"
#include "TaskQueryEntryExit.h"
#include "TaskAddTimestamp.h"

#include <QDebug>

Task* _get_task(QString task, QObject* parent) {
    if (task == "read_dataset") {
        return new TaskReadDataset(parent);
    }
    if (task == "delay") {
        return new TaskDelay(parent);
    }
    if (task == "query_entry_exit") {
        return new TaskQueryEntryExit(parent);
    }
    return nullptr;
}

Task* Task::get_from_factory(QString task, QVariantList args, QObject* parent) {
    Task* _task = _get_task(task, parent);
    if (_task == nullptr)
        qDebug() << task << " not found!";
    else
        _task->args(args);
    return _task;
}
