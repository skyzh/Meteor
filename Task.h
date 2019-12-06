#ifndef TASK_H
#define TASK_H

#include <QThread>
#include <QSqlQuery>
#include <QVariantList>
#include <QVariant>
#include <QList>

class Task : public QThread {
Q_OBJECT

signals:

    void progress(qreal percent);

    void message(QString msg);

    void success(bool ok);

public:
    Task(QObject *parent = nullptr);

    virtual bool journal();

    virtual QList<Task*> dependencies();

    virtual QString name();

    virtual QString display_name();

    virtual void args(QVariantList args);

    void cancel();

protected:
    std::atomic<bool> _cancel;

    virtual void run() override;

    ~Task() override;

    QVariantList _args;

    QVariant get_arg(int idx);

    void emit_sql_error(QString type, QSqlQuery &q);

    void emit_db_error(QString type, QSqlDatabase &db);

    virtual bool parse_args();
};

#endif // TASK_H
