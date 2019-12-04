#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "Task.h"

#include <QThread>
#include <QList>
#include <QMutex>

class TaskScheduler : public QThread {
Q_OBJECT


    QMutex _tasks_mutex;
    QList<Task *> tasks;
    bool task_running;
    QString current_task_name;

    void _schedule();

    bool init_journal();

    bool is_journaled(QString journal_id);

    bool do_journal(QString journal_id);

    void ready();

signals:

    void progress(qreal percent);

    void message(QString msg);

private slots:

    void on_progress(qreal percent);

    void on_message(QString msg);

    void on_success(bool ok);

    void do_schedule();

public:
    TaskScheduler(QObject *parent = nullptr);

    void run() override;

    void schedule(Task *task);

    void schedule(QString task, QVariantList args = QVariantList());

    ~TaskScheduler() override;
};

#endif // TASKSCHEDULER_H
