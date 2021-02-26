cmake_minimum_required(VERSION 3.17)
project(webserver)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(tests)

file(GLOB_RECURSE webserver_sources CONFIGURE_DEPENDS "src/*.cpp")

add_executable(webserver ${webserver_sources})

# TODO: should use target_include_directories to only build files needed for the executable. However, this will only
#   show compilation errors for files that are needed, so for development it's easiest to use file globbing
#target_include_directories(webserver PRIVATE src/)
