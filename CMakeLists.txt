cmake_minimum_required(VERSION 2.8)
project(Panorama)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories ("headers")
include_directories (${OPENCV_INCLUDE_DIR})

add_executable(panorama
    panorama.cpp
    src/arguments.cpp
    src/fast_detector.cpp
    src/transformation.cpp
    src/local_maxima.cpp
)
target_link_libraries (panorama ${OpenCV_LIBS})