//
// Created by Kirby Linvill on 2/25/21.
//

#include "gtest/gtest.h"

#include <string>

#include "../src/http_message.h"


TEST(HttpMessageTest, MessageBuildsHeaderFromContentLength) {
    std::string test_content = "Hello world!";
    HttpMessage test_message = HttpMessage(200, "OK", ContentType::txt, test_content);

    EXPECT_EQ(test_message.header.content_length, test_content.length());
}

TEST(HttpMessageTest, MessageString) {
    std::string test_content = "Hello world!";
    HttpMessage test_message = HttpMessage(200, "OK", ContentType::txt, test_content);

    std::string expected_string ="HTTP/1.1 200 OK\r\nContent-Type: text/txt\r\nContent-Length: 12\r\n\r\nHello world!\r\n";

    EXPECT_EQ(std::string(test_message), expected_string);
}
