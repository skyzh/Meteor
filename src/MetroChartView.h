//
// Created by Alex Chi on 2019/12/07.
//

#ifndef METRO_METROCHARTVIEW_H
#define METRO_METROCHARTVIEW_H

#include <QChartView>

class MetroChartView : public QtCharts::QChartView {
protected:
    void wheelEvent(QWheelEvent *event) override;
};


#endif //METRO_METROCHARTVIEW_H
