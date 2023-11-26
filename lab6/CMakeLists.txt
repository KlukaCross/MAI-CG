cmake_minimum_required(VERSION 3.16)
project(lab6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenGL REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets OpenGL)

add_executable(lab6 main.cpp main_widget.hpp geometry_engine.hpp opengl_widget.hpp main_widget.cpp geometry_engine.cpp opengl_widget.cpp)
target_link_libraries(lab6 PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::OpenGL
        )

configure_file(./vshader.glsl ./vshader.glsl COPYONLY)
configure_file(./fshader.glsl ./fshader.glsl COPYONLY)