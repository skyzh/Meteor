#include "mainwindow.h"

#ifdef DISABLE_TOUCHBAR
void bindTouchbar(MainWindow &window) {}
void setTouchbarSliderFlowValue(int value) {}
#else
