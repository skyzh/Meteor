QT       += core gui
QT       += sql
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/ConfigManager.cpp \
    src/MetroPainter.cpp \
    src/MetroWidget.cpp \
    src/Task.cpp \
    src/TaskAddTimestamp.cpp \
    src/TaskDelay.cpp \
    src/TaskFlowAnalysis.cpp \
    src/TaskGetMapping.cpp \
    src/TaskInitDatabase.cpp \
    src/TaskPlanRoute.cpp \
    src/TaskQueryEntryExit.cpp \
    src/TaskReadDataset.cpp \
    src/TaskScheduler.cpp \
    src/db.cpp \
    src/main.cpp \
    src/main.mm \
    src/mainwindow.cpp \
    src/utils.cpp

HEADERS += \
    src/ConfigManager.h \
    src/MetroPainter.h \
    src/MetroWidget.h \
    src/Task.h \
    src/TaskAddTimestamp.h \
    src/TaskDelay.h \
    src/TaskFlowAnalysis.h \
    src/TaskGetMapping.h \
    src/TaskInitDatabase.h \
    src/TaskPlanRoute.h \
    src/TaskQueryEntryExit.h \
    src/TaskReadDataset.h \
    src/TaskScheduler.h \
    src/db.h \
    src/mainwindow.h \
    src/utils.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
