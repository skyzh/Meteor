//
// Created by Alex Chi on 2019/12/07.
//

#include "MetroChartView.h"
#include <QDebug>
#include <QtMath>

void MetroChartView::wheelEvent(QWheelEvent *event) {
    chart()->scroll(-event->pixelDelta().x(), 0);
    QChartView::wheelEvent(event);
    emit request_data();
}

MetroChartView::MetroChartView(QtCharts::QChart *chart, QWidget *parent) : QChartView(chart, parent) {}

MetroChartView::MetroChartView(QWidget *parent) : QChartView(parent) {}
