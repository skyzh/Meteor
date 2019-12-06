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
    qlonglong id;
    qreal x, y;
};

class MetroPainter {
private:
    QBrush background;
    QBrush stationBrush, stationBrushInner;
    QPen textPen;
    QFont textFont;
    qreal CAMERA_X, CAMERA_Y;
    QList<MetroStation> stations;

public:
    void paint(QPainter *painter, QPaintEvent *event);

    MetroPainter();

    qreal get_camera_x();

    qreal get_camera_y();

    void set_camera_pos(qreal x, qreal y);

    void setStations(QList<MetroStation> stations);
};


#endif //METRO_METROPAINTER_H
