cmake_minimum_required(VERSION 3.9)
project(CS426Assignment4 C)

set(CMAKE_C_STANDARD 11)

add_executable(dispatcher dispatcher.c)

add_executable(process sigtrap.c)