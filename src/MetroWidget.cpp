//
// Created by Alex Chi on 2019/12/06.
//

#include "MetroWidget.h"

#include <QDebug>

MetroWidget::MetroWidget(MetroPainter *helper, QWidget *parent) :
        QWidget(parent), helper(helper) {
    setAutoFillBackground(false);
}

void MetroWidget::animate() {
    update();
}

void MetroWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    helper->paint(&painter, event);
    painter.end();
}

MetroWidget::~MetroWidget() {

}

void MetroWidget::wheelEvent(QWheelEvent *event) {
    auto delta = event->pixelDelta();
    auto x = helper->get_camera_x() - delta.x();
    auto y = helper->get_camera_y() - delta.y();
    helper->set_camera_pos(x, y);
    update();
}

void MetroWidget::setStations(QList<MetroStation> stations, QList<MetroSegment> segments) {
    helper->setStations(stations, segments);
    update();
}
