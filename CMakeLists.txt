cmake_minimum_required(VERSION 3.30)
project(ProiectPOO)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProiectPOO main.cpp sqlite3.c
)

target_include_directories(ProiectPOO PRIVATE ../Downloads)
