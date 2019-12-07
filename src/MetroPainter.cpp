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

    // [2] Draw Station Segments

    // Outer Circle
    painter->setBrush(Qt::BrushStyle::NoBrush);

    for (auto &&segment : segments) {
        segmentPen.setColor(line_color_mapping(segment.lineID));
        painter->setPen(segmentPen);
        painter->drawLine(
                segment.x1 + CAMERA_OFFSET_X,
                segment.y1 + CAMERA_OFFSET_Y,
                segment.x2 + CAMERA_OFFSET_X,
                segment.y2 + CAMERA_OFFSET_Y);
    }

    // [3] Draw Station Circles

    // Outer Circle
    painter->setPen(Qt::PenStyle::NoPen);

    for (auto &&station : stations) {
        stationBrush.setColor(line_color_mapping(station.lineID));
        painter->setBrush(stationBrush);
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

    // [4] Draw Station Name

    painter->setPen(textPen);
    painter->setFont(textFont);
    for (auto &&station : stations) {
        QMatrix matrix;
        matrix.translate(station.x + CAMERA_OFFSET_X,
                         station.y + STATION_CIRCLE_RADIUS + STATION_NAME_MARGIN + CAMERA_OFFSET_Y);
        matrix.rotate(45);
        painter->setMatrix(matrix);
        painter->drawText(QPointF(0, 0),
                          QString("(%1) %2").arg(station.id).arg(station.name));
    }
    painter->resetMatrix();
    painter->restore();
}

MetroPainter::MetroPainter() {
    background = QBrush(Qt::white);
    stationBrush = QBrush(Qt::red);
    stationBrushInner = QBrush(Qt::white);
    segmentPen = QPen(Qt::white);
    segmentPen.setWidth(DEFAULT_SEGMENT_WIDTH);
    textPen = QPen(Qt::black);
    textFont.setPixelSize(12);
    CAMERA_X = CAMERA_Y = 0;
    CAMERA_BOUND = QRectF(0, 0, 0, 0);
}

qreal MetroPainter::get_camera_x() {
    return CAMERA_X;
}

qreal MetroPainter::get_camera_y() {
    return CAMERA_Y;
}

void MetroPainter::set_camera_pos(qreal x, qreal y) {
    if (x < CAMERA_BOUND.x()) x = CAMERA_BOUND.x();
    if (x > CAMERA_BOUND.right()) x = CAMERA_BOUND.right();
    if (y < CAMERA_BOUND.y()) y = CAMERA_BOUND.y();
    if (y > CAMERA_BOUND.bottom()) y = CAMERA_BOUND.bottom();
    CAMERA_X = x;
    CAMERA_Y = y;
}

void MetroPainter::setStations(QList<MetroStation> stations, QList<MetroSegment> segments) {
    this->stations = stations;
    this->segments = segments;
    calc_camera_bound();
    set_camera_pos(get_camera_x(), get_camera_y());
}

QColor MetroPainter::line_color_mapping(QString line) {
    if (line == "B") return QColor(202, 45, 92);
    if (line == "A") return QColor(130, 192, 90);
    if (line == "C") return QColor(240, 136, 62);
    return QColor(0, 0, 0);
}

void MetroPainter::calc_camera_bound() {
    qreal x1, x2, y1, y2;
    x1 = x2 = y1 = y2 = 0;
    const qreal MARGIN = 100;
    if (!stations.empty()) {
        x2 = x1 = stations.first().x;
        y2 = y1 = stations.first().y;
        for (auto &&station:stations) {
            x1 = qMin(x1, station.x);
            x2 = qMax(x2, station.x);
            y1 = qMin(y1, station.y);
            y2 = qMax(y2, station.y);
        }
    }
    CAMERA_BOUND = QRectF(
            x1 - MARGIN,
            y1 - MARGIN,
            x2 - x1 + MARGIN * 2,
            y2 - y1 + MARGIN * 2);
}
