//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_TASKPLANROUTE_H
#define METRO_TASKPLANROUTE_H

#include "Task.h"

#include <QMutex>

class TaskPlanRoute : public Task {
    Q_OBJECT

private:
    unsigned long long from, to;

    QList<qulonglong> data;

    QMutex _data_mutex;

public:
    TaskPlanRoute(QObject *parent = nullptr);

    bool journal() override;

    void run() override;

    QString name() override;

    QString display_name() override;

    ~TaskPlanRoute() override;

    QList<qulonglong> get_data();

    static QList<qulonglong> plan_route(const QVector<QVector<int>> &adj_mat, qulonglong N, qulonglong from, qlonglong to);

    static QVector<QVector<int>> get_route_mapping();

signals:
    void result();

protected:
    bool parse_args() override;
};


#endif //METRO_TASKPLANROUTE_H
