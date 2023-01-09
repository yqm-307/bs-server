cmake_minimum_required(VERSION 3.2)

project(Servers)

set(${CMAKE_CXX_STANDARD} "-std=c++17")

include_directories(
    ${PROJECT_SOURCE_DIR}/mainserver
    ${PROJECT_SOURCE_DIR}/subserver
    ${PROJECT_SOURCE_DIR}/share
)

add_subdirectory(subserver)
add_subdirectory(mainserver)