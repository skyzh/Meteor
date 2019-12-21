cmake_minimum_required(VERSION 3.12)
project(Metro)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT APPLE)
    add_definitions(-DDISABLE_TOUCH_BAR)
    set(TOUCHBAR_SRCS src/Touchbar.cpp)
else()
    set(TOUCHBAR_SRCS src/Touchbar.mm)
endif()

find_package(Qt5 COMPONENTS Widgets Sql Charts REQUIRED)

add_executable(Meteor
        src/db.cpp
        src/db.h
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        ${TOUCHBAR_SRCS}
        src/Task.cpp
        src/Task.h
        src/TaskDelay.cpp
        src/TaskDelay.h
        src/TaskReadDataset.cpp
        src/TaskReadDataset.h
        src/TaskScheduler.cpp
        src/TaskScheduler.h
        src/mainwindow.ui
        src/TaskQueryEntryExit.cpp
        src/TaskQueryEntryExit.h
        src/TaskAddTimestamp.cpp
        src/TaskAddTimestamp.h
        src/TaskInitDatabase.cpp
        src/TaskInitDatabase.h
        src/TaskPlanRoute.cpp
        src/TaskPlanRoute.h
        src/ConfigManager.cpp
        src/ConfigManager.h
        src/MetroWidget.cpp
        src/MetroWidget.h
        src/MetroPainter.cpp
        src/MetroPainter.h
        src/TaskGetMapping.cpp
        src/TaskGetMapping.h
        src/TaskFlowAnalysis.cpp
        src/TaskFlowAnalysis.h
        src/utils.cpp
        src/utils.h
        src/TouchBar.h
        src/MetroChartView.cpp
        src/MetroChartView.h
        src/TaskQueryFlow.cpp
        src/TaskQueryFlow.h
        src/TaskSmartTravel.cpp
        src/TaskSmartTravel.h
        src/TaskQuerySmartTravel.cpp
        src/TaskQuerySmartTravel.h
        src/TaskBaseFlowAnalysis.cpp
        src/TaskBaseFlowAnalysis.h
        src/wizard.h
        src/wizard.cpp
        src/wizard.ui 
        src/mainwindowTouchBar.cpp 
        src/mainwindow_tab_flow.cpp 
        src/mainwindow_tab_route.cpp 
        src/mainwindow_tab_passenger.cpp)

if (APPLE)
    target_link_libraries(Meteor PRIVATE Qt5::Core Qt5::Sql Qt5::Widgets Qt5::Charts "-framework AppKit")
else()
    target_link_libraries(Meteor PRIVATE Qt5::Core Qt5::Sql Qt5::Widgets Qt5::Charts)
endif()


