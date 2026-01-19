cmake_minimum_required(VERSION 4.0)
project(projet_poo)

set(CMAKE_CXX_STANDARD 20)

add_executable(projet_poo
    src/main.cpp
    src/Table.h
    src/Table.cpp
    src/Database.h
    src/Database.cpp
    src/Bibliotheque.h
    src/Adherent.h
    src/Livre.h
)
