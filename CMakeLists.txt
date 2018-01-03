cmake_minimum_required(VERSION 3.7)
project(executor)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES es2sequenziale.c)
add_executable(executor ${SOURCE_FILES})