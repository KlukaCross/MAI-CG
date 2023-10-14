cmake_minimum_required(VERSION 3.16)
project(lab2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(lab2 main.cpp main_window.cpp main_window.hpp prism.hpp prism.cpp)
target_link_libraries(lab2
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )