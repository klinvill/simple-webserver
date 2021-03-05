//
// Data structures for HTTP Request and Response messages
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

    // status code
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
    // parses the header from a string message
    // can throw invalid_argument exception
    explicit HttpRequestHeader(const std::string& data);

    RequestType type;
    std::string resource;
    int content_length;
    HttpVersion version;
    ConnectionDirective connection_directive;

private:
    // helper functions to parse the string representation of a header
    static std::string get_next_field(const std::string& data, unsigned long& offset);
    static KeyValue get_next_key_value(const std::string& data, unsigned long& offset);
};

class HttpRequestMessage {
public:
    // parses the http message from its string representation
    // can throw invalid_argument exception
    explicit HttpRequestMessage(const std::string& data);

    HttpRequestHeader header;
    std::string content;
};


#endif //WEBSERVER_HTTP_MESSAGE_H
