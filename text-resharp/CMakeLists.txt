cmake_minimum_required(VERSION 3.29)
project(text_resharp)

set(CMAKE_CXX_STANDARD 20)

add_library(
        text_resharp SHARED
        library.cpp
        common/conditions.h
        common/header.h
        linux/libs.h
        linux/source.cpp
        windows/libs.h
        windows/source.cpp
)
