//
// Created by Alex Chi on 2019/12/06.
//

#include "MetroWidget.h"

#include <QDebug>

MetroWidget::MetroWidget(MetroPainter *helper, QWidget *parent) :
        QWidget(parent), helper(helper),
        animation_x(this, "camera_x"),
        animation_y(this, "camera_y"){
    setAutoFillBackground(false);
    connect(this, &MetroWidget::camera_moved, this, &MetroWidget::on_camera_moved);
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
    m_camera_x = x;
    m_camera_y = y;
    helper->set_camera_pos(x, y);
    update();
}

void MetroWidget::setStations(QVector<MetroStation> stations, QVector<MetroSegment> segments) {
    helper->setStations(stations, segments);
    update();
}

void MetroWidget::set_camera_pos(qreal x, qreal y) {
    animation_x.stop();
    animation_y.stop();

    animation_x.setStartValue(m_camera_x);
    animation_x.setEndValue(x);
    animation_x.setDuration(500);
    animation_x.setEasingCurve(QEasingCurve::InOutQuad);

    animation_y.setStartValue(m_camera_y);
    animation_y.setEndValue(y);
    animation_y.setDuration(500);
    animation_y.setEasingCurve(QEasingCurve::InOutQuad);

    animation_x.start();
    animation_y.start();
}

void MetroWidget::on_camera_moved() {
    helper->set_camera_pos(m_camera_x, m_camera_y);
    update();
}
