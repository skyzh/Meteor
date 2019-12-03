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
    void cancel();

protected:
    std::atomic<bool> _cancel;
    virtual void run() override;
    ~Task() override;
};

#endif // TASK_H
