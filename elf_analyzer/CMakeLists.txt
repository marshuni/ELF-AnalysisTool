cmake_minimum_required(VERSION 3.10)
project(ELFAnalyzerC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 包含头文件路径
include_directories(include json)

# 所有源文件
file(GLOB SRC_FILES
    src/*.c
    src/parsers/*.c
    src/json/cJSON.c
    main.c
)

# 生成可执行文件
add_executable(elf-analyzer ${SRC_FILES})
