//
// Created by Alex Chi on 2019/12/07.
//

#include "MetroChartView.h"

void MetroChartView::wheelEvent(QWheelEvent *event) {
    chart()->scroll(event->pixelDelta().x(), event->pixelDelta().y());
}
