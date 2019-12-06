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
        TaskReadDataset.cpp
        TaskReadDataset.h
        TaskScheduler.cpp
        TaskScheduler.h
        mainwindow.ui TaskQueryEntryExit.cpp TaskQueryEntryExit.h TaskAddTimestamp.cpp TaskAddTimestamp.h TaskInitDatabase.cpp TaskInitDatabase.h)

target_link_libraries(Meteor PRIVATE Qt5::Core Qt5::Sql Qt5::Widgets Qt5::Charts)