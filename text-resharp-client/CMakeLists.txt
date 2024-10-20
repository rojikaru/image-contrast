cmake_minimum_required(VERSION 3.29)
project(text_resharp_client)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        text_resharp_client
        main.cpp
        common/conditions.h
        common/header.h
        linux/libs.h
        linux/source.cpp
        windows/libs.h
        windows/source.cpp
)
