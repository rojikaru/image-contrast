cmake_minimum_required(VERSION 3.28)
project(image_contrast)

set(CMAKE_CXX_STANDARD 20)

if (UNIX)
    set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/build/")
elseif (WIN32)
    set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/build/x64/vc16/lib")
endif ()
message("OpenCV directory: " ${OpenCV_DIR})

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME}
        windows/libs.h
        linux/libs.h
        common/conditions.h
        common/header.h
        windows/source.cpp
        linux/source.cpp
        main.cpp)

target_link_libraries(
        ${PROJECT_NAME}
        ${OpenCV_LIBS}
)
