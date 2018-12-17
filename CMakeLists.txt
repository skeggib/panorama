cmake_minimum_required(VERSION 2.8)
project(Panorama)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

include_directories ("headers")
include_directories (${OPENCV_INCLUDE_DIR})

add_executable(panorama
    panorama.cpp
    src/arguments.cpp
    src/corner_detection.cpp
    src/transformation.cpp
)
target_link_libraries (panorama ${OpenCV_LIBS})

add_executable(corner_detection
    corner_detection.cpp
    src/arguments.cpp
    src/corner_detection.cpp
)
target_link_libraries (corner_detection ${OpenCV_LIBS})

add_executable(corner_pairing
    corner_pairing.cpp
    src/arguments.cpp
    src/corner_detection.cpp
)
target_link_libraries (corner_pairing ${OpenCV_LIBS})
