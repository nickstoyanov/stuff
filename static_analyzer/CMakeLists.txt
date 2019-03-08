cmake_minimum_required(VERSION 2.8.9)
project(saproject)

########################################################################################################################
if(UNIX AND NOT APPLE)
    set(PLATFORM linux)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/")
    set(CMAKE_BUILD_RPATH "$ORIGIN/")
elseif(APPLE)
    set(PLATFORM mac)
endif()
########################################################################################################################

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -O3")

add_executable(saproject ${SOURCES} ${HEADERS})
