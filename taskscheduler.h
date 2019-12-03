#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "task.h"

#include <QThread>
#include <QList>
#include <QMutex>

class TaskScheduler : public QThread
{
    Q_OBJECT


    QMutex _tasks_mutex;
    QList <Task* > tasks;
    bool task_running;
    QString current_task_name;

    void _schedule();
    bool init_journal();
    bool is_journaled(QString journal_id);
    bool do_journal(QString journal_id);

signals:
    void progress(qreal percent);
    void message(QString msg);
    void ready(bool ok);

private slots:
    void on_progress(qreal percent);
    void on_message(QString msg);
    void on_success(bool ok);
    void do_schedule();

public:
    TaskScheduler(QObject* parent = nullptr);
    void run() override;
    void schedule(Task *task);
    ~TaskScheduler() override;
};

#endif // TASKSCHEDULER_H
