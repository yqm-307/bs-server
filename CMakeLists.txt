cmake_minimum_required(VERSION 3.2)

project(Servers)

set(Wall_Flag "-Wall -Wno-sign-compare -Wno-format -Wno-reorder")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -std=c++17 -g ${Wall_Flag}")


include_directories(
    ${PROJECT_SOURCE_DIR}
)
message(${PROJECT_SOURCE_DIR})
# add_subdirectory(subserver)
add_subdirectory(mainserver)