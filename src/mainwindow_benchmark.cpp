//
// Created by Alex Chi on 2019/12/25.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "TaskQueryEntryExit.h"
#include "TaskFlowAnalysis.h"
#include "TaskSmartTravel.h"

#include <QDebug>

void MainWindow::on_actionBenchmark_triggered() {
    {
        auto taskReadDataset = new TaskReadDataset(this);
        taskReadDataset->args({"2019-01-09"});
        do_benchmark("read_dataset", taskReadDataset);
    }
    {
        for (int i = 0; i < 10; i++) {
            auto taskQuery = new TaskQueryEntryExit(this);
            taskQuery->args({
                                    300,
                                    1546963200 + 8 * 60 * 60,
                                    1546963200 + 8 * 60 * 60 + 3600,
                                    ui->comboLine->currentData(),
                                    ui->comboStation->currentData()
                            });
            do_benchmark("entry_exit_hour", taskQuery);
        }
    }
    {
        for (int i = 0; i < 10; i++) {
            auto taskQuery = new TaskQueryEntryExit(this);
            taskQuery->args({
                                    1800,
                                    1546963200,
                                    1546963200 + 86400,
                                    ui->comboLine->currentData(),
                                    ui->comboStation->currentData()
                            });
            do_benchmark("entry_exit_day", taskQuery);
        }
    }
    {
        auto taskReadDataset = new TaskReadDataset(this);
        taskReadDataset->args({"2019-01-10"});
        do_benchmark("read_dataset", taskReadDataset);
    }
    {
        auto taskFlow = new TaskFlowAnalysis(this);
        taskFlow->args({
                               1546963200,
                               1546963200 + 86400
                       });
        do_benchmark("flow_analysis", taskFlow);
    }
    {
        for (int i = 0; i < 10; i++) {
            auto taskFlow = new TaskQueryFlow(this);
            taskFlow->args({
                                   1546963200,
                                   1546963200 + 86400
                           });
            do_benchmark("query_flow", taskFlow);
        }
    }
    {
        for (int i = 0; i < 50; i++) {
            auto task = new TaskPlanRoute(this);
            task->args({0, 50});
            do_benchmark("route_plan", task);
        }
    }
    {
        auto taskTravel = new TaskSmartTravel(this);
        taskTravel->args({
                                 1546963200,
                                 1546963200 + 86400
                         });
        do_benchmark("smart_travel", taskTravel);
    }
    {
        for (int i = 0; i < 10; i++) {
            auto taskTravel = new TaskQuerySmartTravel(this);
            taskTravel->args({
                                     1546963200 + 8 * 60 * 60
                             });
            taskTravel->route_stations = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            do_benchmark("query_smart_travel", taskTravel);
        }
    }
}

void MainWindow::do_benchmark(QString title, Task *task) {
    auto start_time = QDateTime::currentDateTime();
    connect(task, &Task::success, [=]() {
        auto end_time = QDateTime::currentDateTime();
        qDebug() << QString(">%1|%2|%3").arg(title).arg(task->name()).arg(start_time.msecsTo(end_time));
    });
    scheduler.schedule(task);
}
