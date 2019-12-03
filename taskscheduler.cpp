#include "taskscheduler.h"
#include "db.h"

#include <QEventLoop>
#include <QMetaObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

TaskScheduler::TaskScheduler(QObject* parent) : QThread(parent), task_running(false) {

}

TaskScheduler::~TaskScheduler() {
}

bool TaskScheduler::do_journal(QString journal_id) {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit message(QString("Failed to open SQL connection: %1").arg(db.lastError().text()));
        return false;
    }

    QSqlQuery q(db);
    q.prepare("insert into journal (journal_id, complete_at) values (:journal_id, :complete_at)");
    q.bindValue(":journal_id", journal_id);
    q.bindValue(":complete_at", QDateTime::currentSecsSinceEpoch());
    if (!q.exec()) {
        emit message(QString("SQL Error: %1").arg(q.lastError().text()));
        return false;
    }
    return true;
}

bool TaskScheduler::is_journaled(QString journal_id) {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit message(QString("Failed to open SQL connection: %1").arg(db.lastError().text()));
        return false;
    }

    QSqlQuery q(db);
    q.prepare("select count(*) from journal where journal_id=:journal_id");
    q.bindValue(":journal_id", journal_id);
    if (!q.exec()) {
        emit message(QString("SQL Error: %1").arg(q.lastError().text()));
        return false;
    }
    q.next();
    return q.value(0).toInt() != 0;
}

void TaskScheduler::run() {
    if (!init_journal()) return;
    QEventLoop loop;
    emit ready(true);
    loop.exec();
    if (task_running) {
        auto task = tasks.first();
        task->cancel();
        task->quit();
        task->wait();
    }
}

bool TaskScheduler::init_journal() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit message(QString("Failed to open SQL connection: %1").arg(db.lastError().text()));
        return false;
    }

    QSqlQuery q(db);

    const QString JOURNAL_INIT = \
            "create table if not exists journal (journal_id text primary key, complete_at int)";
    if (!q.exec(JOURNAL_INIT)) {
        emit message(QString("SQL Error: %1").arg(q.lastError().text()));
        return false;
    }
    return true;
}

void TaskScheduler::_schedule() {
    QMutexLocker l(&_tasks_mutex);
    if (task_running) return;
    while(!tasks.empty()) {
        auto task = tasks.first();
        if (task->journal() && is_journaled(task->name()))
            tasks.pop_front();
        else
            break;
    }
    if (tasks.empty()) {
        emit ready(true);
        return;
    }
    task_running = true;
    auto task = tasks.first();
    connect(task, &Task::progress, this, &TaskScheduler::on_progress);
    connect(task, &Task::message, this, &TaskScheduler::on_message);
    connect(task, &Task::success, this, &TaskScheduler::on_success);
    task->start();
    current_task_name = task->name();
    qDebug() << "running " << task->name();
    emit ready(false);
}

void TaskScheduler::schedule(Task* task) {
    _tasks_mutex.lock();
    connect(task, &Task::finished, task, &QObject::deleteLater);
    tasks.push_back(task);
    _tasks_mutex.unlock();
    QMetaObject::invokeMethod(this, &TaskScheduler::do_schedule);
}

void TaskScheduler::on_progress(qreal percent) {
    emit progress(percent);
}

void TaskScheduler::on_message(QString msg) {
    emit message(QString("%1: %2").arg(current_task_name).arg(msg));
}

void TaskScheduler::on_success(bool ok) {
    _tasks_mutex.lock();
    auto task = tasks.first();
    if (task->journal()) do_journal(task->name());
    tasks.pop_front();
    task_running = false;
    _tasks_mutex.unlock();
    if (ok) _schedule();
}

void TaskScheduler::do_schedule() {
    _schedule();
}

