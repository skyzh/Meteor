//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_METROPAINTER_H
#define METRO_METROPAINTER_H

#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QFont>

struct MetroStation {
    QString name;
    qulonglong id;
    qreal x, y;
    QString lineID;
    QString msg;
};

struct MetroSegment {
    qreal x1, y1, x2, y2;
    QColor upper_color, lower_color;
    QString upper_msg, lower_msg;
};

class MetroPainter {
private:
    QBrush background;
    QBrush stationBrush, stationBrushInner;
    QPen textPen;
    QFont textFont;
    QPen segmentPen;
    qreal CAMERA_X, CAMERA_Y;
    QVector<MetroStation> stations;
    QVector<MetroSegment> segments;

    const int DEFAULT_SEGMENT_WIDTH = 8;

    QRectF CAMERA_BOUND;

    void calc_camera_bound();

public:
    void paint(QPainter *painter, QPaintEvent *event);

    MetroPainter();

    qreal get_camera_x();

    qreal get_camera_y();

    void set_camera_pos(qreal x, qreal y);

    void setStations(
            QVector<MetroStation> stations,
            QVector<MetroSegment> segments);

    static QColor line_color_mapping(QString line);
};


#endif //METRO_METROPAINTER_H
