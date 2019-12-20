//
// Created by Alex Chi on 2019/12/08.
//

#include "TaskQuerySmartTravel.h"
#include "TaskSmartTravel.h"
#include "TaskPlanRoute.h"

#include "db.h"

#include <QDebug>
#include <queue>

using std::priority_queue;

bool TaskQuerySmartTravel::journal() {
    return false;
}

QList<Task *> TaskQuerySmartTravel::dependencies() {
    auto task = new TaskSmartTravel(this);
    task->args({start_time, start_time + 86400});
    return {task};
}

QString TaskQuerySmartTravel::name() {
    return "query_smart_travel";
}

QString TaskQuerySmartTravel::display_name() {
    return "Estimating Travel Time";
}

void TaskQuerySmartTravel::run() {
    DB _db;
    QSqlDatabase db = _db.get();

    if (!db.open()) {
        emit_db_error("Open SQL connection", db);
        return;
    }

    QSqlQuery q(db);

    // [2] Pre-process distance
    emit message("Obtaining station distance");
    preprocess_distance_mapping();

    // [3] Fetch Data from Database
    emit message("Analyzing");
    q.prepare("select * from smart_travel where start_time=:start_time");
    q.bindValue(":start_time", start_time);

    unsigned long long depature_time_block = (departure_time - start_time) / time_div;

    if (!q.exec()) {
        emit_sql_error("Error when querying", q);
        return;
    }

    QVector<priority_queue<SmartTravelRecord>> neighbours_vec;
    const int K = 10;

    for (auto &&route : route_stations) neighbours_vec.push_back({});

    auto enter_station_id = route_stations[0];

    while (q.next()) {
        for (int i = 0; i < route_stations.size(); i++) {
            auto &neighbours = neighbours_vec[i];
            auto exit_station_id = route_stations[i];

            auto _enter = q.value(1).toUInt();
            auto _exit = q.value(2).toUInt();
            auto _tb = q.value(3).toUInt();
            auto delta_enter = distance[_enter][enter_station_id] * 10;
            auto delta_exit = distance[_exit][exit_station_id] * 10;
            auto delta_departure = _tb - depature_time_block;
            auto delta_n = 1.0 / q.value(5).toUInt() * 5;
            double cost = delta_enter * delta_enter + delta_exit * delta_exit + delta_departure * delta_departure +
                          delta_n * delta_n;
            travel_time = double(q.value(4).toUInt()) / q.value(5).toUInt();
            auto record = SmartTravelRecord{
                    _enter, _exit, _tb, travel_time, cost
            };
            if (neighbours.size() < K) {
                neighbours.push(record);
            } else {
                if (neighbours.top().cost > cost) {
                    neighbours.pop();
                    neighbours.push(record);
                }
            }
        }
    }

    for (int i = 0; i < route_stations.size(); i++) {
        auto &neighbours = neighbours_vec[i];
        double sum = 0; int n = 0;
        while (!neighbours.empty()) {
            auto x = neighbours.top();
            neighbours.pop();
            // if (x.cost > 300) continue;
            sum += x.travel_time;
            n += 1;
        }
        if (n == 0) {
            estimated_time.push_back(0);
        } else {
            estimated_time.push_back(sum / n);
        }
    }

    for (int i = 0; i < route_stations.size() - 1; i++) {
        if (estimated_time[i + 1] < estimated_time[i]) estimated_time[i] = 0;
    }

    if (!_cancel) {
        emit result();
        emit success(true);
    } else {
        emit success(false);
    }

    db.close();
}

TaskQuerySmartTravel::~TaskQuerySmartTravel() {

}

bool TaskQuerySmartTravel::parse_args() {
    departure_time = get_arg(0).toUInt();
    QDateTime timestamp;
    timestamp.setTime_t(departure_time);
    timestamp.setTime(QTime(4, 0));
    start_time = timestamp.toSecsSinceEpoch();
    return true;
}

TaskQuerySmartTravel::TaskQuerySmartTravel(QObject *parent) : Task(parent) {
}

void TaskQuerySmartTravel::preprocess_distance_mapping() {
    auto mat = TaskPlanRoute::get_route_mapping();
    int N = mat.size();
    for (int i = 0; i < N; i++) {
        distance.push_back({});
        for (int j = 0; j < N; j++) {
            distance[i] << TaskPlanRoute::plan_route(mat, N, i, j).size() - 1;
        }
    }
}

QVector<double> TaskQuerySmartTravel::get_estimated_time() {
    return estimated_time;
}

bool operator<(const SmartTravelRecord &a, const SmartTravelRecord &b) { return a.cost < b.cost; }