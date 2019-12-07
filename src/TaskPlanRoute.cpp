//
// Created by Alex Chi on 2019/12/06.
//

#include "TaskPlanRoute.h"
#include "ConfigManager.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

TaskPlanRoute::TaskPlanRoute(QObject *parent) : Task(parent) {

}

bool TaskPlanRoute::journal() {
    return false;
}

struct Route {
    qulonglong id;
    qlonglong parent;
};

QList<qulonglong> TaskPlanRoute::plan_route(const QVector<QVector<int>> &adj_mat, qulonglong N, qulonglong from, qlonglong to) {
    if (from < 0 || to < 0 || from >= N || to >= N) return QList<qulonglong>();
    if (from == to) return QList<qulonglong>();

    QList<qulonglong> route;
    QList<Route> q;
    QList<bool> visited;
    for (int i = 0; i < N; i++) visited << false;
    visited[from] = true;
    q << Route { from, -1 };
    qlonglong head = 0;
    while (head < q.length()) {
        auto last_step = q[head];
        if (last_step.id == to) break;
        for (qulonglong i = 0; i < N; i++) {
            if (!visited[i]) {
                if (adj_mat[last_step.id][i]) {
                    visited[i] = true;
                    q << Route { i, head };
                }
            }
        }
        ++head;
    }

    while (head != -1) {
        route.push_front(qulonglong(q[head].id));
        head = q[head].parent;
    }

    return route;
}

void TaskPlanRoute::run() {
    {
        QMutexLocker l(&_data_mutex);
        QVector<QVector<int>> mat = get_route_mapping();
        if (mat.empty()) {
            emit message("Error while reading road map");
            emit success(false);
            return;
        }
        data = plan_route(mat, mat.length(), from, to);
    }

    emit result();

    emit success(true);
}

QString TaskPlanRoute::name() {
    return "plan_route";
}

QString TaskPlanRoute::display_name() {
    return "Plan Route";
}

TaskPlanRoute::~TaskPlanRoute() {

}

bool TaskPlanRoute::parse_args() {
    from = get_arg(0).toULongLong();
    to = get_arg(1).toULongLong();
    return true;
}

QList<qulonglong> TaskPlanRoute::get_data() {
    QMutexLocker l(&_data_mutex);
    return data;
}

QVector<QVector<int>> TaskPlanRoute::get_route_mapping() {
    const QString path = QString("%1/adjacency_adjacency/Metro_roadMap.csv").arg(ConfigManager::instance()->get("DATASET_PATH").toString());
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    QTextStream in(&file);

    QVector<QVector<int>> mat;
    bool first_line = true;

    while (!in.atEnd()) {
        QStringList list = in.readLine().split(",");
        if (first_line) {
            first_line = false;
            continue;
        }
        list.removeFirst();
        mat.push_back({});
        for (auto &&i : list) {
            if (i == "1") mat.last() << 1;
            else mat.last() << 0;
        }
    }
    return mat;
}
