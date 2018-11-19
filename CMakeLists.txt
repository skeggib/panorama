cmake_minimum_required(VERSION 2.8)
project(Panorama)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories ("headers")
include_directories (${OPENCV_INCLUDE_DIR})

add_executable(fast_detector_test
    src/main.cpp
    src/fast_detector.cpp
    src/distance.cpp
    src/local_maxima.cpp
    src/skeletonization.cpp
    src/arguments.cpp
)
target_link_libraries (fast_detector_test ${OpenCV_LIBS})
