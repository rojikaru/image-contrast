cmake_minimum_required(VERSION 3.29)
project(os_lab1)

set(CMAKE_CXX_STANDARD 20)

add_executable(os_lab1 main.cpp
        windows/libs.h
        common/conditions.h
        common/header.h
        windows/source.cpp)
