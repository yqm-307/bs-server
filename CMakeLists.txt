cmake_minimum_required(VERSION 3.1)

project(Servers)

set(Wall_Flag "-Wall -Wno-sign-compare -Wno-format -Wno-reorder -Wno-unused")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -std=c++17 -g ${Wall_Flag}")


include_directories(
    ${PROJECT_SOURCE_DIR}
)

link_directories(
    /usr/local/lib
)

message(${PROJECT_SOURCE_DIR})
add_subdirectory(mainserver)

add_subdirectory(subserver)