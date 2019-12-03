#include "Task.h"

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

QString Task::display_name() {
    return name();
}

void Task::run() {
}

void Task::args(QStringList args) {
    _args = args;
}

Task::~Task() {}

void Task::cancel() {
    _cancel = true;
}

QString Task::get_arg(int idx) {
    if (idx < _args.size())
        return _args[idx];
    else
        return "";
}
