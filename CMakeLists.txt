cmake_minimum_required(VERSION 3.0.0)
project(glWrapper VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(glWrapper STATIC
source/glWrapper.cpp
libs/tinygltf/stb.cpp
libs/tinygltf/tinygltf.cpp
libs/gl/glad.c
)

target_include_directories(glWrapper
PRIVATE "${CMAKE_SOURCE_DIR}/libs/"
PRIVATE "${CMAKE_SOURCE_DIR}/libs/gl"
PRIVATE "${CMAKE_SOURCE_DIR}/libs/glm"
PRIVATE "${CMAKE_SOURCE_DIR}/libs/tinygltf"
PRIVATE "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(glWrapper PRIVATE "${CMAKE_SOURCE_DIR}/libs/gl/libglfw3.a")

add_executable(testProj test/main.cpp)

target_include_directories(testProj
PRIVATE "${CMAKE_SOURCE_DIR}/include"
PRIVATE "${CMAKE_SOURCE_DIR}/libs"
PRIVATE "${CMAKE_SOURCE_DIR}/libs/gl"
PRIVATE "${CMAKE_SOURCE_DIR}/libs/glm"
PRIVATE "${CMAKE_SOURCE_DIR}/libs/tinygltf"
)

target_link_libraries(testProj PRIVATE glWrapper)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)