#include "task.h"

Task::Task(QObject *parent) : QThread(parent), _cancel(false) {
}

bool Task::journal() {
    return false;
}

QStringList Task::dependencies() {
    return QStringList();
}

QString Task::name() {
    return "not implemented";
}

void Task::run() {
}

Task::~Task() {}

void Task::cancel() {
    _cancel = true;
}
