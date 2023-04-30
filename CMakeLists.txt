cmake_minimum_required(VERSION 3.0.0)
project(glWrapper VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY &{CMAKE_BINARY_DIR}/test)

add_library(glWrapper source/glWrapper.cpp)

add_executable(testProj test/main.cpp)

target_include_directories(testProj PRIVATE {CMAKE_BINARY_DIR}/include)

target_link_libraries(testProj PRIVATE glWrapper)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)