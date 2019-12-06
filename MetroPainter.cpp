//
// Created by Alex Chi on 2019/12/06.
//

#include "MetroPainter.h"
#include <QDebug>

void MetroPainter::paint(QPainter *painter, QPaintEvent *event) {
    const qreal STATION_CIRCLE_RADIUS = 10;
    const qreal STATION_CIRCLE_INNER_RADIUS = 6;
    const qreal STATION_NAME_MARGIN = 5;

    painter->save();

    // [1] Draw Background
    auto region = event->rect();
    const qreal CAMERA_OFFSET_X = -CAMERA_X + region.width() / 2;
    const qreal CAMERA_OFFSET_Y = -CAMERA_Y + region.height() / 2;
    painter->fillRect(region, background);

    // [2] Draw Station Circles

    // Outer Circle
    painter->setPen(Qt::PenStyle::NoPen);
    painter->setBrush(stationBrush);

    for (auto &&station : stations) {
        QPointF pos = QPointF(station.x + CAMERA_OFFSET_X, station.y + CAMERA_OFFSET_Y);
        painter->drawEllipse(pos, STATION_CIRCLE_RADIUS, STATION_CIRCLE_RADIUS);
    }

    // Inner Circle
    painter->setPen(Qt::PenStyle::NoPen);
    painter->setBrush(stationBrushInner);

    for (auto &&station : stations) {
        QPointF pos = QPointF(station.x + CAMERA_OFFSET_X, station.y + CAMERA_OFFSET_Y);
        painter->drawEllipse(pos, STATION_CIRCLE_INNER_RADIUS, STATION_CIRCLE_INNER_RADIUS);
    }

    // [3] Draw Station Name

    painter->setPen(textPen);
    painter->setFont(textFont);
    for (auto &&station : stations) {
        QRect pos = QRect(station.x + CAMERA_OFFSET_X - 100,
                          station.y + STATION_CIRCLE_RADIUS + STATION_NAME_MARGIN + CAMERA_OFFSET_Y,
                          200,
                          80);
        painter->drawText(pos,
                          Qt::AlignHCenter | Qt::AlignTop,
                          station.name);
    }

    painter->restore();
}

MetroPainter::MetroPainter() {
    background = QBrush(Qt::white);
    stationBrush = QBrush(Qt::red);
    stationBrushInner = QBrush(Qt::white);
    textPen = QPen(Qt::black);
    textFont.setPixelSize(12);
    CAMERA_X = CAMERA_Y = 0;
}

qreal MetroPainter::get_camera_x() {
    return CAMERA_X;
}

qreal MetroPainter::get_camera_y() {
    return CAMERA_Y;
}

void MetroPainter::set_camera_pos(qreal x, qreal y) {
    CAMERA_X = x;
    CAMERA_Y = y;
}

void MetroPainter::setStations(QList<MetroStation> stations) {
    this->stations = stations;
}
