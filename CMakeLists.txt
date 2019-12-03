cmake_minimum_required(VERSION 3.15)
project(Metro)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Sql Charts REQUIRED)

add_executable(Metro
        db.cpp
        db.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        Task.cpp
        Task.h
        TaskDelay.cpp
        TaskDelay.h
        TaskFactory.cpp
        TaskQuery.cpp
        TaskQuery.h
        TaskReadDataset.cpp
        TaskReadDataset.h
        TaskScheduler.cpp
        TaskScheduler.h
        mainwindow.ui TaskQueryEntryExit.cpp TaskQueryEntryExit.h)

target_link_libraries(Metro PRIVATE Qt5::Core Qt5::Sql Qt5::Widgets Qt5::Charts)