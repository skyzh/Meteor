//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_METROWIDGET_H
#define METRO_METROWIDGET_H

#include "MetroPainter.h"

#include <QWidget>
#include <QOpenGLWidget>
#include <QTimer>
#include <QPropertyAnimation>

class MetroWidget : public QWidget {
Q_OBJECT

public:
    MetroWidget(MetroPainter *helper, QWidget *parent);

    virtual ~MetroWidget();

    void setStations(QVector<MetroStation> stations, QVector<MetroSegment> segments);

    void set_camera_pos(qreal x, qreal y);

    Q_PROPERTY(qreal camera_x
                       MEMBER
                       m_camera_x
                       NOTIFY
                       camera_moved)
    Q_PROPERTY(qreal camera_y
                       MEMBER
                       m_camera_y
                       NOTIFY
                       camera_moved)

signals:
    void camera_moved();

protected:
    void paintEvent(QPaintEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

private:
    MetroPainter *helper;
    QEasingCurve easing;
    QPropertyAnimation animation_x;
    QPropertyAnimation animation_y;
    qreal m_camera_x, m_camera_y;

    void on_camera_moved();
};


#endif //METRO_METROWIDGET_H
