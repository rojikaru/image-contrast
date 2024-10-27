cmake_minimum_required(VERSION 3.29)
project(image_contrast_client)

set(CMAKE_CXX_STANDARD 20)
set(IMAGE_CONTRAST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../image-contrast")
set(IMAGE_CONTRAST_BIN "${IMAGE_CONTRAST_DIR}/cmake-build-debug")

add_executable(
        ${PROJECT_NAME}
        main.cpp
        common/conditions.h
        common/header.h
)

# OpenCV
if (WIN32)
    set(OpenCV_DIR "D:/opencv/build")
    set(OpenCV_DLLS "${OpenCV_DIR}/x64/vc16/bin")

    file(GLOB OpenCV_DLLS "${OpenCV_DLLS}/*.dll")
    file(COPY ${OpenCV_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else ()
    set(OpenCV_DIR "/usr/local/opencv/build")
endif ()

find_package(OpenCV REQUIRED)

include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${IMAGE_CONTRAST_DIR}
        ${OpenCV_INCLUDE_DIRS}
)
link_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${IMAGE_CONTRAST_BIN}
)
link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        ${OpenCV_LIBS}
)