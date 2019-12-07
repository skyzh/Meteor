cmake_minimum_required(VERSION 3.15)
project(Metro)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets Sql Charts REQUIRED)

add_executable(Meteor
        src/db.cpp
        src/db.h
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
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
        src/main.mm src/TaskPlanRoute.cpp src/TaskPlanRoute.h src/ConfigManager.cpp src/ConfigManager.h src/MetroWidget.cpp src/MetroWidget.h src/MetroPainter.cpp src/MetroPainter.h src/TaskGetMapping.cpp src/TaskGetMapping.h src/TaskFlowAnalysis.cpp src/TaskFlowAnalysis.h src/utils.cpp src/utils.h)

target_link_libraries(Meteor PRIVATE Qt5::Core Qt5::Sql Qt5::Widgets Qt5::Charts "-framework AppKit")
