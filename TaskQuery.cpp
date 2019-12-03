#include "TaskQuery.h"

TaskQuery::TaskQuery(QObject *parent) : Task(parent) {

}

bool TaskQuery::journal() {
    return false;
}

QString TaskQuery::name() {
    return "query";
}
