cmake_minimum_required(VERSION 2.8)

if (CMAKE_BUILD_TYPE STREQUAL Release) 
	set(MyProjectName robots)
else()
	set(CMAKE_BUILD_TYPE Debug)
	set(MyProjectName robotsd)
endif()

message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")
message(STATUS "ProjectName:${MyProjectName}")

project(${MyProjectName})

aux_source_directory(. SRCS)
aux_source_directory(../../libs/libserver SRCS)
aux_source_directory(../../libs/libserver/protobuf SRCS)
aux_source_directory(../../libs/libserver/mongoose SRCS)
aux_source_directory(../../libs/libplayer SRCS)
aux_source_directory(../../libs/libresource SRCS)

include_directories(../../libs)
include_directories(../../../../include/common)
include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../../bin")

set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -pthread -lprotobuf -ljsoncpp -lyaml-cpp -llog4cplus -lhiredis -luuid -DEPOLL")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

link_directories(${CMAKE_BINARY_DIR}/../../../libs)
link_directories(${CMAKE_BINARY_DIR}/../../../../libs/linux)

add_executable(${MyProjectName} ${SRCS})
#target_link_libraries(${MyProjectName} libserverd.a)
