cmake_minimum_required(VERSION 3.29)
project(image_contrast_dynamic_client)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        image_contrast_dynamic_client
        main.cpp
        common/conditions.h
        common/header.h
)

set(IMAGE_CONTRAST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../image-contrast")
set(IMAGE_CONTRAST_BIN "${IMAGE_CONTRAST_DIR}/cmake-build-debug")

if (WIN32)
    add_definitions(-DIMAGE_CONTRAST_DLL_IMPORTS)

    # Copy Image Contrast DLLs to the output directory
    file(GLOB ImageContrast_DLLS "${IMAGE_CONTRAST_BIN}/*.dll")
    file(COPY ${ImageContrast_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else ()
    file(GLOB ImageContrast_DLLS "${IMAGE_CONTRAST_BIN}/*.so")
    file(COPY ${ImageContrast_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

target_include_directories(image_contrast_dynamic_client PRIVATE ${IMAGE_CONTRAST_DIR})
target_link_directories(image_contrast_dynamic_client PRIVATE ${IMAGE_CONTRAST_BIN})
