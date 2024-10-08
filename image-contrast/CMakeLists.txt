cmake_minimum_required(VERSION 3.28)
project(image_contrast)

set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/build/")
message("OpenCV directory: " ${OpenCV_DIR})

if (WIN32)
    # Copy OpenCV DLLs to the output directory
    file(GLOB OpenCV_DLLS "${OpenCV_DIR}/x64/vc16/bin/*.dll")
    file(COPY ${OpenCV_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

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
