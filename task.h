#ifndef TASK_H
#define TASK_H

#include <QThread>

class Task : public QThread {
Q_OBJECT

signals:

    void progress(qreal percent);

    void message(QString msg);

    void success(bool ok);

public:
    Task(QObject *parent = nullptr);

    virtual bool journal();

    virtual QStringList dependencies();

    virtual QString name();

    virtual QString display_name();

    virtual void args(QStringList args);

    void cancel();

    static Task *get_from_factory(QString task, QStringList args = QStringList(), QObject *parent = nullptr);

protected:
    std::atomic<bool> _cancel;

    virtual void run() override;

    ~Task() override;

    QStringList _args;

    QString get_arg(int idx);
};

#endif // TASK_H
