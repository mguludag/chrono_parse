cmake_minimum_required(VERSION 3.14)
project(chrono_parse_example VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt"
        GIT_TAG "9.1.0"
)

FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG "v2.4.11"
)

FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(DocTest)

add_executable(${PROJECT_NAME} example/main.cpp)
target_link_libraries(${PROJECT_NAME} fmt::fmt)
target_include_directories(${PROJECT_NAME} AFTER PUBLIC ${CMAKE_SOURCE_DIR}/include)

enable_testing()
add_subdirectory(test)
