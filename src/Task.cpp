#include "Task.h"

#include <QSqlError>

Task::Task(QObject *parent) : QThread(parent), _cancel(false) {
}

bool Task::journal() {
    return false;
}

QList<Task*> Task::dependencies() {
    return QList<Task*>();
}

QString Task::name() {
    return "not implemented";
}

QString Task::display_name() {
    return name();
}

void Task::run() {
}

void Task::args(QVariantList args) {
    _args = args;
    parse_args();
}

Task::~Task() {}

void Task::cancel() {
    _cancel = true;
}

QVariant Task::get_arg(int idx) {
    if (idx < _args.size())
        return _args[idx];
    else
        return QVariant();
}

void Task::emit_sql_error(QString type, QSqlQuery &q) {
    emit message(QString("%1: %2").arg(type).arg(q.lastError().text()));
    emit success(false);
}

void Task::emit_db_error(QString type, QSqlDatabase &db) {
    emit message(QString("%1: %2").arg(type).arg(db.lastError().text()));
    emit success(false);
}

bool Task::parse_args() {
    return true;
}
