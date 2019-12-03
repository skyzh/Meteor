#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scheduler(this)
{
    ui->setupUi(this);

    connect(&scheduler, &TaskScheduler::progress, this, &MainWindow::progress);
    connect(&scheduler, &TaskScheduler::message, this, &MainWindow::message);
    connect(&scheduler, &TaskScheduler::ready, this, &MainWindow::success);
    scheduler.start();
}

MainWindow::~MainWindow()
{
    scheduler.quit();
    scheduler.wait();
    delete ui;
}
void MainWindow::progress(qreal percent) {
    ui->progressBar->setValue(int(percent * 100));
}

void MainWindow::message(QString msg) {
    ui->label->setText(msg);
}

void MainWindow::success(bool ok) {
    qDebug() << ok;
}

void MainWindow::on_pushButton_clicked() {
    scheduler.schedule(new TaskReadDataset(this));
}
