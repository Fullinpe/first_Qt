cmake_minimum_required(VERSION 3.16)
project(first_Qt)

set(CMAKE_CXX_STANDARD 14)

link_directories(ffmpeg-4.2.2/lib)

include_directories(ffmpeg-4.2.2/include/libavcodec ffmpeg-4.2.2/include/libavdevice ffmpeg-4.2.2/include/libavfilter ffmpeg-4.2.2/include/libavformat
        ffmpeg-4.2.2/include/libavutil ffmpeg-4.2.2/include/libpostproc ffmpeg-4.2.2/include/libswresample ffmpeg-4.2.2/include/libswscale ffmpeg-4.2.2/include)


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

target_link_libraries(
        first_Qt
        Qt5::Widgets
        avformat
        avdevice
        avfilter
        avcodec
        avutil
        postproc
        swscale
        swresample
        #        avcodec-58 avdevice-58 avfilter-7 avformat-58 avutil-56 postproc-55 swresample-3 swscale-5
)

qt5_use_modules(first_Qt Core Widgets Gui)
