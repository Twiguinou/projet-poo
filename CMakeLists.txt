cmake_minimum_required(VERSION 3.31)

project(projet_poo VERSION 1.0.0)

option(BUILD_UI "Build the Qt graphical interface" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(library)

if (BUILD_UI)
    add_subdirectory(ui)
endif ()
