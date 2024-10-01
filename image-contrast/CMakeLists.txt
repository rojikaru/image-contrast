cmake_minimum_required(VERSION 3.29)
project(image_contrast)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(OpenCV REQUIRED)

add_executable(image_contrast main.cpp
        windows/libs.h
        common/conditions.h
        common/header.h
        windows/source.cpp
        linux/source.cpp)

target_link_libraries(image_contrast ${CONAN_LIBS} ${OpenCV_LIBS})
