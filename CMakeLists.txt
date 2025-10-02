cmake_minimum_required(VERSION 3.10)
project(PulseBench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include src)

file(GLOB SRC_FILES
    src/*.cpp
)

add_executable(pulsebench ${SRC_FILES})

find_package(Threads REQUIRED)
target_link_libraries(pulsebench PRIVATE Threads::Threads)
