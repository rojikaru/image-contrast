cmake_minimum_required(VERSION 3.28)
project(image_contrast)

set(CMAKE_CXX_STANDARD 20)
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/build/")
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        windows/libs.h
        common/conditions.h
        common/header.h
        windows/source.cpp
        linux/source.cpp
        main.cpp
        linux/source.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
