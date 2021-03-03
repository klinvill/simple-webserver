//
// Created by Kirby Linvill on 2/25/21.
//

#ifndef WEBSERVER_HTTP_MESSAGE_H
#define WEBSERVER_HTTP_MESSAGE_H

#include <string>

#include "connection_directive.h"
#include "content_type.h"
#include "http_version.h"
#include "request_type.h"
#include "util_types.h"


class HttpResponseHeader {
public:
    HttpResponseHeader(int status, std::string status_string, ContentType content_type, int content_length,
                       HttpVersion version, ConnectionDirective connection_directive=ConnectionDirective());
    explicit operator std::string() const;

    int status;
    std::string status_string;
    ContentType content_type;
    int content_length;
    HttpVersion version;
    ConnectionDirective connection_directive;
};

class HttpResponseMessage {
public:
    HttpResponseMessage(int status, std::string status_string, ContentType content_type, std::string content,
                        HttpVersion version, ConnectionDirective connection_directive);
    explicit operator std::string() const;

    HttpResponseHeader header;
    std::string content;
};

class HttpRequestHeader {
public:
    explicit HttpRequestHeader(const std::string& data);

    RequestType type;
    std::string resource;
    int content_length;
    HttpVersion version;
    ConnectionDirective connection_directive;

private:
    static std::string get_next_field(const std::string& data, unsigned long& offset);
    static KeyValue get_next_key_value(const std::string& data, unsigned long& offset);
};

class HttpRequestMessage {
public:
    explicit HttpRequestMessage(const std::string& data);

    HttpRequestHeader header;
    std::string content;
};


#endif //WEBSERVER_HTTP_MESSAGE_H
