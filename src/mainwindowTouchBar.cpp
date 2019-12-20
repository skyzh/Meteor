//
// Created by Alex Chi on 2019/12/20.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::tb_buttonTabQuery_clicked() {
    ui->tabWidget->setCurrentWidget(ui->tab_passenger_traffic);
}

void MainWindow::tb_buttonTabRoutePlanning_clicked() {
    ui->tabWidget->setCurrentWidget(ui->tab_route_planning);
}

void MainWindow::tb_buttonTabFlow_clicked() {
    ui->tabWidget->setCurrentWidget(ui->tab_flow);
}

void MainWindow::tb_sliderFlow_changed(int value) {
    ui->sliderTime->setValue(value);
    update_flow_position(value);
}