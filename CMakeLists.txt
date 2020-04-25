cmake_minimum_required(VERSION 3.16)
project(first_Qt)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)

set(UI_FILES)

set(RC_FILES)

set(QRC_FILES)

#set(SOURCE_FILES main.cpp mainwindow.cpp mainwindow.h)

add_executable(first_Qt main.cpp widget.cpp widget.h ${RC_FILES}${UI_FILES}${QRC_FILES})

target_link_libraries(first_Qt Qt5::Widgets)

qt5_use_modules(first_Qt Core Widgets Gui)