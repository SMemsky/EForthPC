cmake_minimum_required(VERSION 2.6)

project(eforthpc)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2.4 REQUIRED COMPONENTS system window graphics)
include_directories(${SFML_INCLUDE_DIR})

include_directories(source)

file(GLOB_RECURSE SOURCES source/*.cpp)

add_definitions(-std=c++14 -Wall -Wextra -Werror -Wpedantic)

add_executable(eforthpc ${SOURCES})

target_link_libraries(eforthpc
	${SFML_LIBRARIES}
)
