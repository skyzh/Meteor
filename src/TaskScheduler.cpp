#include "TaskScheduler.h"
#include "db.h"

#include <QEventLoop>
#include <QMetaObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

TaskScheduler::TaskScheduler(QObject *parent)
        : QThread(parent), task_running(false),
          task_cnt(0), task_cnt_total(0) {

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
    qDebug() << journal_id << " journaled";
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
    do_schedule();
    loop.exec();
    if (task_running) {
        auto task = tasks.first();
        task.task->cancel();
        task.task->quit();
        task.task->wait();
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
    if (task_running) return;
    while (!tasks.empty()) {
        auto task = tasks.first().task;
        if (task->journal() && is_journaled(task->name())) {
            tasks.pop_front();
            ++task_cnt;
        } else
            break;
    }
    if (tasks.empty()) {
        ready();
        return;
    }
    task_running = true;
    auto task = tasks.first().task;
    connect(task, &Task::progress, this, &TaskScheduler::on_progress);
    connect(task, &Task::message, this, &TaskScheduler::on_message);
    connect(task, &Task::success, this, &TaskScheduler::on_success);
    current_task_name = task->display_name();
    emit_message();
    emit progress(0.0);
    task->start();
}

void TaskScheduler::on_progress(qreal percent) {
    emit progress(percent);
}

void TaskScheduler::on_message(QString msg) {
    emit_message(msg);
}

void TaskScheduler::on_success(bool ok) {
    QMutexLocker l(&_tasks_mutex);
    auto task = tasks.first().task;
    if (ok && task->journal()) do_journal(task->name());
    tasks.pop_front();
    task_running = false;
    ++task_cnt;
    if (ok) _schedule();
    else {
        emit progress(0);
    }
}

void TaskScheduler::do_schedule() {
    QMutexLocker l(&_tasks_mutex);
    _schedule();
}

void TaskScheduler::ready() {
    emit message("Ready");
    emit progress(1.0);
    task_cnt_total = 0;
    task_cnt = 0;
}

void TaskScheduler::schedule(Task *task) {
    {
        QMutexLocker l(&_tasks_mutex);
        connect(task, &Task::finished, task, &QObject::deleteLater);
        resolve(task);
    }
    QMetaObject::invokeMethod(this, &TaskScheduler::do_schedule);
}

void TaskScheduler::emit_message(QString msg) {
    int percent = int(double(task_cnt + 1) / task_cnt_total * 100);
    if (percent != 100) {
        emit message(QString("(%3%) %1%2")
                             .arg(current_task_name)
                             .arg(msg == "" ? "" : ": " + msg)
                             .arg(percent)
        );
    } else {
        emit message(QString("%1%2")
                             .arg(current_task_name)
                             .arg(msg == "" ? "" : ": " + msg)
        );
    }
}

void TaskScheduler::resolve(Task *task, QString parent) {
    QList<Task *> dependencies = task->dependencies();
    QString name = task->display_name();
    for (Task *dependency: dependencies) {
        resolve(dependency, name);
    }
    if (!parent.isEmpty()) {
        tasks.push_back({task, QString("before %1").arg(parent)});
    } else {
        tasks.push_back({task});
    }
    ++task_cnt_total;
}

bool TaskScheduler::running() {
    QMutexLocker l(&_tasks_mutex);
    return task_running;
}
