cmake_minimum_required(VERSION 3.29)
project(image_contrast)

set(CMAKE_CXX_STANDARD 20)

add_executable(image_contrast main.cpp
        windows/libs.h
        common/conditions.h
        common/header.h
        windows/source.cpp
        linux/source.cpp)

if (WIN32)
    target_link_libraries(image_contrast PRIVATE gdiplus)
endif()
