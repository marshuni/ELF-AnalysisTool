cmake_minimum_required(VERSION 3.10)
project(ELFAnalyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件路径
include_directories(include json)

# 递归收集源文件
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(elf-analyzer main.cpp ${SOURCES})
