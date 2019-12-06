//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_TASKINITDATABASE_H
#define METRO_TASKINITDATABASE_H

#include "Task.h"

class TaskInitDatabase : public Task {
public:
    TaskInitDatabase(QObject *parent = nullptr);

    bool journal() override;

    void run() override;

    QString name() override;

    QString display_name() override;

    ~TaskInitDatabase() override;
};


#endif //METRO_TASKINITDATABASE_H
