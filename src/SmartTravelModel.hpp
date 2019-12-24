//
// Created by Alex Chi on 2019/12/23.
//

#ifndef METRO_SMARTTRAVELMODEL_HPP
#define METRO_SMARTTRAVELMODEL_HPP

#include <queue>
#include <QVector>

struct SmartTravelRecord {
    unsigned long long enter_station_id, exit_station_id, enter_time_block;
    double travel_time, cost;
};

inline bool operator<(const SmartTravelRecord &a, const SmartTravelRecord &b) {
    return a.cost < b.cost;
}

class SmartTravelModel {
public:
    SmartTravelModel(unsigned long long int enterStationId, unsigned long long int exitStationId,
                     unsigned long long int timeBlock, const QVector<QVector<int>> &distance);

    void train(unsigned long long int enterStationId, unsigned long long int exitStationId,
               unsigned long long int timeBlock, unsigned long long sum, unsigned long long n);

    double predict();
private:
    const unsigned int K = 10;
    unsigned long long enter_station_id, exit_station_id, time_block;
    const QVector<QVector<int>> &distance;
    std::priority_queue<SmartTravelRecord> neighbours;
};

inline SmartTravelModel::SmartTravelModel(unsigned long long int enterStationId, unsigned long long int exitStationId,
                                          unsigned long long int timeBlock, const QVector<QVector<int>> &distance)
        : enter_station_id(enterStationId), exit_station_id(exitStationId), time_block(timeBlock), distance(distance) {}

inline void SmartTravelModel::train(unsigned long long int enterStationId, unsigned long long int exitStationId,
                                    unsigned long long int timeBlock, unsigned long long sum, unsigned long long n) {
    auto delta_enter = distance[enterStationId][enter_station_id] * 10;
    auto delta_exit = distance[exitStationId][exit_station_id] * 10;
    auto delta_departure = timeBlock - time_block;
    auto delta_n = 1.0 / n * 5;
    double cost = delta_enter * delta_enter + delta_exit * delta_exit + delta_departure * delta_departure +
                  delta_n * delta_n;
    double travel_time = double(sum) / n;
    auto record = SmartTravelRecord{
            enterStationId, exitStationId, timeBlock, travel_time, cost
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

inline double SmartTravelModel::predict() {
    double sum = 0; int n = 0;
    while (!neighbours.empty()) {
        auto x = neighbours.top();
        neighbours.pop();
        // if (x.cost > 300) continue;
        sum += x.travel_time;
        n += 1;
    }
    if (n == 0) {
        return 0;
    } else {
        return sum / n;
    }
}

#endif //METRO_SMARTTRAVELMODEL_HPP
