cmake_minimum_required(VERSION 3.14)

project(
  chrono_parse
  VERSION 1.0
  LANGUAGES CXX)

option(CHRONO_PARSE_BUILD_DOCS "Build documentation" OFF)
option(CHRONO_PARSE_BUILD_EXAMPLE "Build example" ON)
option(CHRONO_PARSE_NO_INSTALL "Skip installation of enum_name" OFF)
option(CHRONO_PARSE_NO_TESTS "Skip testing of enum_name" OFF)

# Define the library
add_library(chrono_parse INTERFACE)
add_library(mgutility::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Specify the include directories for the library
target_include_directories(
  chrono_parse INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mgutility/include>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:mgutility/include>
)

# Set the C++ standard
target_compile_features(chrono_parse INTERFACE cxx_std_11)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  include(GNUInstallDirs)
  set(include_install_dir ${CMAKE_INSTALL_INCLUDEDIR})
  set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
else()
  set(include_install_dir "include")
  set(config_install_dir "lib/cmake/${PROJECT_NAME}")
endif()

# Create the package configuration files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/chrono_parseConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/chrono_parseConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/chrono_parseConfig.cmake"
  INSTALL_DESTINATION lib/cmake/chrono_parse)

if(NOT ${CHRONO_PARSE_NO_INSTALL})
  # Install the library
  install(
    TARGETS chrono_parse
    EXPORT chrono_parseTargets
    INCLUDES
    DESTINATION include)

  install(
    EXPORT chrono_parseTargets
    FILE chrono_parseTargets.cmake
    NAMESPACE mgutility::
    DESTINATION lib/cmake/chrono_parse)

  install(DIRECTORY include/ DESTINATION include)

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/chrono_parseConfig.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/chrono_parseConfigVersion.cmake"
          DESTINATION lib/cmake/chrono_parse)

  export(
    EXPORT chrono_parseTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/chrono_parseTargets.cmake"
    NAMESPACE mgutility::)
endif()

if(${CHRONO_PARSE_BUILD_EXAMPLE})
  # FetchContent to get the fmt library
  include(FetchContent)
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 8.1.1) # Change the tag to the desired version
  FetchContent_MakeAvailable(fmt)

  # Add example executable
  add_executable(example_chrono_parse example/main.cpp)

  # Link the example executable with the library
  target_link_libraries(example_chrono_parse PRIVATE mgutility::chrono_parse fmt::fmt)
endif()

if(NOT ${CHRONO_PARSE_NO_TESTS})
  # Enable testing
  enable_testing()

  # FetchContent to get the Doctest testing framework
  include(FetchContent)
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11)
  FetchContent_MakeAvailable(doctest)

  # Add test executable
  add_executable(test_chrono_parse tests/test_chrono_parse.cpp)

  # Link the test executable with the library and Doctest
  target_link_libraries(test_chrono_parse PRIVATE mgutility::chrono_parse
                                                  doctest::doctest)

  # Add tests
  add_test(NAME test_chrono_parse COMMAND test_chrono_parse)
endif()

if(${CHRONO_PARSE_BUILD_DOCS})
  add_subdirectory(doc)
endif()