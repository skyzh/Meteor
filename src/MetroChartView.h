//
// Created by Alex Chi on 2019/12/07.
//

#ifndef METRO_METROCHARTVIEW_H
#define METRO_METROCHARTVIEW_H

#include <QChartView>

class MetroChartView : public QtCharts::QChartView {
    Q_OBJECT
protected:
    void wheelEvent(QWheelEvent *event) override;

public:
    MetroChartView(QWidget *parent);

    MetroChartView(QtCharts::QChart *chart, QWidget *parent);

signals:
    void request_data();
};


#endif //METRO_METROCHARTVIEW_H
