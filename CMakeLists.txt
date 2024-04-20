    cmake_minimum_required(VERSION 3.24)
project(pagetables C)

set(CMAKE_C_STANDARD 11)

add_executable(pagetables main.c config.h mlpt.h)
