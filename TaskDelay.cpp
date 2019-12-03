#include "TaskDelay.h"

TaskDelay::TaskDelay(QObject *parent) : Task(parent) {

}

TaskDelay::~TaskDelay() {

}

QString TaskDelay::display_name() {
    return QString("Delay %1ms").arg(get_arg(0).toInt());
}

QString TaskDelay::name() {
    return "delay";
}

bool TaskDelay::journal() {
    return false;
}

void TaskDelay::run() {
    int N = _args[0].toInt();
    for (int i = 0; i < N; i++) {
        QThread::msleep(10);
        emit progress((i + 1) / double(N));
        if (_cancel) break;
    }
    emit success(true);
}
