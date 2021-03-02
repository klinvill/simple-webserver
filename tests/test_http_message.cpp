//
// Created by Kirby Linvill on 2/25/21.
//

#include "gtest/gtest.h"

#include <string>

#include "../src/http_message.h"


TEST(HttpResponseMessageTest, MessageBuildsHeaderFromContentLength) {
    std::string test_content = "Hello world!";
    HttpResponseMessage test_message = HttpResponseMessage(200, "OK", ContentType::txt, test_content, HttpVersion(HttpVersionEnum::HTTP_1_1));

    EXPECT_EQ(test_message.header.content_length, test_content.length());
}

TEST(HttpResponseMessageTest, MessageString) {
    std::string test_content = "Hello world!";
    HttpResponseMessage test_message = HttpResponseMessage(200, "OK", ContentType::txt, test_content, HttpVersion(HttpVersionEnum::HTTP_1_1));

    std::string expected_string ="HTTP/1.1 200 OK\r\nContent-Type: text/txt\r\nContent-Length: 12\r\n\r\nHello world!\r\n";

    EXPECT_EQ(std::string(test_message), expected_string);
}

TEST(HttpResponseMessageTest, BuildHttp_1_0_Message) {
    std::string test_content = "Hello world!";
    HttpResponseMessage test_message = HttpResponseMessage(200, "OK", ContentType::txt, test_content, HttpVersion(HttpVersionEnum::HTTP_1_0));

    std::string expected_string ="HTTP/1.0 200 OK\r\nContent-Type: text/txt\r\nContent-Length: 12\r\n\r\nHello world!\r\n";

    EXPECT_EQ(std::string(test_message), expected_string);
}


TEST(HttpRequestMessageTest, ParseGetMessageFromString) {
    std::string test_string ="GET /hello/world HTTP/1.1\r\n\r\n";
    HttpRequestMessage test_message = HttpRequestMessage(test_string);
    std::string expected_resource = "/hello/world";

    EXPECT_EQ(test_message.header.type, RequestType(RequestTypeEnum::GET));
    EXPECT_EQ(test_message.header.resource, expected_resource);
    EXPECT_EQ(test_message.header.content_length, 0);
    EXPECT_EQ(test_message.header.version, HttpVersion(HttpVersionEnum::HTTP_1_1));
    EXPECT_EQ(test_message.content, "");
}

TEST(HttpRequestMessageTest, ParsePostMessageFromString) {
    std::string test_string ="POST /hello/world HTTP/1.1\r\nContent-Length: 12\r\n\r\nHello world!\r\n";
    HttpRequestMessage test_message = HttpRequestMessage(test_string);
    std::string expected_resource = "/hello/world";
    std::string expected_content = "Hello world!";

    EXPECT_EQ(test_message.header.type, RequestType(RequestTypeEnum::POST));
    EXPECT_EQ(test_message.header.resource, expected_resource);
    EXPECT_EQ(test_message.header.content_length, 12);
    EXPECT_EQ(test_message.header.version, HttpVersion(HttpVersionEnum::HTTP_1_1));
    EXPECT_EQ(test_message.content, expected_content);
}

TEST(HttpRequestMessageTest, ParseMessageWithNewlinesInContent) {
    std::string test_string ="POST /hello/world HTTP/1.1\r\nContent-Length: 13\r\n\r\nHello\r\nworld!\r\n";
    HttpRequestMessage test_message = HttpRequestMessage(test_string);
    std::string expected_resource = "/hello/world";
    std::string expected_content = "Hello\r\nworld!";

    EXPECT_EQ(test_message.header.type, RequestType(RequestTypeEnum::POST));
    EXPECT_EQ(test_message.header.resource, expected_resource);
    EXPECT_EQ(test_message.header.content_length, 13);
    EXPECT_EQ(test_message.header.version, HttpVersion(HttpVersionEnum::HTTP_1_1));
    EXPECT_EQ(test_message.content, expected_content);
}

TEST(HttpRequestMessageTest, ParseHttp_1_0_Message) {
    std::string test_string ="GET /hello/world HTTP/1.0\r\n\r\n";
    HttpRequestMessage test_message = HttpRequestMessage(test_string);
    std::string expected_resource = "/hello/world";

    EXPECT_EQ(test_message.header.type, RequestType(RequestTypeEnum::GET));
    EXPECT_EQ(test_message.header.resource, expected_resource);
    EXPECT_EQ(test_message.header.content_length, 0);
    EXPECT_EQ(test_message.header.version, HttpVersion(HttpVersionEnum::HTTP_1_0));
    EXPECT_EQ(test_message.content, "");
}
