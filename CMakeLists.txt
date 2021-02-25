cmake_minimum_required(VERSION 3.17)
project(webserver)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE webserver_sources CONFIGURE_DEPENDS "src/*.cpp")

add_executable(webserver ${webserver_sources})
