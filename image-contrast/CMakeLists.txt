cmake_minimum_required(VERSION 3.28)
project(image_contrast)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(IMAGE_CONTRAST_DLL_EXPORTS False)

if (WIN32)

    set(OpenCV_DIR "D:/opencv/build")

    # Copy OpenCV DLLs to the output directory
    file(GLOB OpenCV_DLLS "${OpenCV_DIR}/x64/vc16/bin/*.dll")
    file(COPY ${OpenCV_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else ()
    set(OpenCV_DIR "~/Documents/opencv/build")
endif ()

message("OpenCV directory: " ${OpenCV_DIR})
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

if (IMAGE_CONTRAST_DLL_EXPORTS)
    add_library(
            ${PROJECT_NAME}
            SHARED
            windows/libs.h
            linux/libs.h
            conditions.h
            image_contrast.h
            windows/source.cpp
            linux/source.cpp
    )
else ()
    add_library(
            ${PROJECT_NAME}
            STATIC
            windows/libs.h
            linux/libs.h
            conditions.h
            image_contrast.h
            windows/source.cpp
            linux/source.cpp
    )
endif ()

# Define MYLIBRARY_EXPORTS when building the shared library
if (IMAGE_CONTRAST_DLL_EXPORTS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IMAGE_CONTRAST_DLL_EXPORTS)

    # Specify the .def file only on Windows
    if(WIN32)
        target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/image_contrast.def)
        message("Adding .def file")
    endif()
endif ()

target_link_libraries(
        ${PROJECT_NAME}
        ${OpenCV_LIBS}
)
