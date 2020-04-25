cmake_minimum_required(VERSION 3.16)
project(first_Qt)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

set(useWin32 TRUE)

if (useWin32)
    add_executable(first_Qt WIN32 main.cpp widget.cpp widget.h)
else ()
    add_executable(first_Qt main.cpp widget.cpp widget.h)
endif ()

target_link_libraries(first_Qt Qt5::Widgets)

qt5_use_modules(first_Qt Core Widgets Gui)
