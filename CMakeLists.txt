cmake_minimum_required(VERSION 3.28)
project(zswriter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/gms/app/Qt/6.7.3/gcc_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(zswriter main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/flowlayout.cpp
        src/flowlayout.h)
target_link_libraries(zswriter
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

