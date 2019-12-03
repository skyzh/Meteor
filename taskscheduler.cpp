#include "TaskScheduler.h"
#include "db.h"

#include <QEventLoop>
#include <QMetaObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

TaskScheduler::TaskScheduler(QObject *parent) : QThread(parent), task_running(false) {

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
    q.prepare("select * from journal where journal_id=:journal_id");
    q.bindValue(":journal_id", journal_id);
    if (!q.exec()) {
        emit message(QString("SQL Error: %1").arg(q.lastError().text()));
        return false;
    }
    if (q.next()) {
        QDateTime timestamp;
        timestamp.setTime_t(q.value("complete_at").toUInt());
        qDebug() << QString("%1 last journaled at %2")
                .arg(journal_id)
                .arg(timestamp.toString(Qt::SystemLocaleShortDate));
        return true;
    } else {
        return false;
    }
}

void TaskScheduler::run() {
    if (!init_journal()) return;
    QEventLoop loop;
    ready();
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
    while (!tasks.empty()) {
        auto task = tasks.first();
        if (task->journal() && is_journaled(task->name()))
            tasks.pop_front();
        else
            break;
    }
    if (tasks.empty()) {
        ready();
        return;
    }
    task_running = true;
    auto task = tasks.first();
    connect(task, &Task::progress, this, &TaskScheduler::on_progress);
    connect(task, &Task::message, this, &TaskScheduler::on_message);
    connect(task, &Task::success, this, &TaskScheduler::on_success);
    current_task_name = task->display_name();
    emit message(current_task_name);
    emit progress(0.0);
    task->start();
}

void TaskScheduler::schedule(Task *task) {
    _tasks_mutex.lock();
    connect(task, &Task::finished, task, &QObject::deleteLater);
    tasks.push_back(task);
    _tasks_mutex.unlock();
    QMetaObject::invokeMethod(this, &TaskScheduler::do_schedule);
}

void TaskScheduler::schedule(QString task, QStringList args) {
    schedule(Task::get_from_factory(task, args, this));
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

void TaskScheduler::ready() {
    emit message("Ready");
    emit progress(1.0);
}