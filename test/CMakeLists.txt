cmake_minimum_required(VERSION 3.14)
project(chrono_parse_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(AFTER PUBLIC  ${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} chrono_parse_test.cpp)
target_link_libraries(${PROJECT_NAME} doctest)

add_test(NAME chrono_parse_test COMMAND chrono_parse_test)