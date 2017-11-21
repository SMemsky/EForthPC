cmake_minimum_required(VERSION 2.6)

project(eforthpc)

include_directories(source)

file(GLOB_RECURSE SOURCES source/*.cpp)

add_definitions(-std=c++14 -Wall -Wextra -Werror -Wpedantic)

add_executable(eforthpc ${SOURCES})

target_link_libraries(eforthpc
)
