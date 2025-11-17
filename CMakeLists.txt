cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
cmake_policy(SET CMP0077 NEW)
project(PulseBench VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include src)

file(GLOB SRC_FILES
    src/*.cpp
)

add_executable(pulsebench ${SRC_FILES})

find_package(Threads REQUIRED)
target_link_libraries(pulsebench PRIVATE Threads::Threads)

# Optional: tests and JSON config parsing
option(BUILD_TESTS "Build unit tests" ON)

include(FetchContent)
if(BUILD_TESTS)
    # Fetch Catch2 for tests
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.10
    )
    FetchContent_MakeAvailable(catch2)

    add_executable(tests tests/test_stats.cpp src/stats.cpp)
    target_include_directories(tests PRIVATE include)
    target_link_libraries(tests PRIVATE Threads::Threads Catch2::Catch2)
endif()

# Fetch nlohmann/json for config parsing
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(pulsebench PRIVATE nlohmann_json::nlohmann_json)
