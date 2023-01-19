set(${CMAKE_CXX_FLAGS} "-g -wall -std=c++17")



set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/Sub)

file(GLOB_RECURSE SRCS
    ${PROJECT_SOURCE_DIR}/share/mysql/*.cc
    ${PROJECT_SOURCE_DIR}/share/mysql/*.hpp
    ${PROJECT_SOURCE_DIR}/share/network/*.cc
    ${PROJECT_SOURCE_DIR}/share/network/*.hpp
    ${PROJECT_SOURCE_DIR}/share/util/*.cc
    ${PROJECT_SOURCE_DIR}/share/util/*.hpp
    ${PROJECT_SOURCE_DIR}/subserver/*.cc
    ${PROJECT_SOURCE_DIR}/subserver/*.hpp
)

# message("this is regex result: " ${SRCS})

add_executable(subserver ${SRCS})
target_link_libraries(subserver -lboost_regex  -lpthread -ljsoncpp -lmysqlclient)