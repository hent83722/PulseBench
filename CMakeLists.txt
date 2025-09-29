cmake_minimum_required(VERSION 3.10)
project(BenchmarkTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(benchmark
    src/main.cpp
    src/benchmark.cpp
    src/utils.cpp
    src/stats.cpp
)
