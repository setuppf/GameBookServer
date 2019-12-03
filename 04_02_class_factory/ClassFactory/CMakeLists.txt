cmake_minimum_required(VERSION 2.8)

if (CMAKE_BUILD_TYPE STREQUAL Release) 
	set(MyProjectName test)
else()
	set(CMAKE_BUILD_TYPE Debug)
	set(MyProjectName testd)
endif()

message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")
message(STATUS "MyProjectName:${MyProjectName}")

project(${MyProjectName})

aux_source_directory(. SRCS)

include_directories(/usr/include)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -pthread -DEPOLL -Wno-unknown-pragmas")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

link_directories(/usr/local/lib)

add_executable(${MyProjectName} ${SRCS})
