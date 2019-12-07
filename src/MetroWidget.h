//
// Created by Alex Chi on 2019/12/06.
//

#ifndef METRO_METROWIDGET_H
#define METRO_METROWIDGET_H

#include "MetroPainter.h"
#include <QOpenGLWidget>

class MetroWidget : public QWidget {
Q_OBJECT

public:
    MetroWidget(MetroPainter *helper, QWidget *parent);

    virtual ~MetroWidget();

    void setStations(QVector<MetroStation> stations, QVector<MetroSegment> segments);

    void set_camera_pos(qreal x, qreal y);
public slots:
    void animate();

protected:
    void paintEvent(QPaintEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

private:
    MetroPainter *helper;
};


#endif //METRO_METROWIDGET_H
