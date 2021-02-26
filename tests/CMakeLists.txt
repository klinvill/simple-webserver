project(tests)

include_directories("/usr/local/include")

find_library(GTestLib gtest)
find_library(GTestMainLib gtest_main)

# TODO: setup library rather than simply using all the sources
add_executable(HttpMessageTest test_http_message.cpp ../src/http_message.cpp ../src/content_type.cpp ../src/file_helpers.cpp)
target_link_libraries(HttpMessageTest ${GTestLib} ${GTestMainLib})
