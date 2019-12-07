//
// Created by Alex Chi on 2019/12/07.
//

#include "utils.h"

QString get_timestamp_str(unsigned long long tsp) {
    QDateTime timestamp;
    timestamp.setTime_t(tsp);
    return timestamp.toString(Qt::SystemLocaleShortDate);
}
