//
// Created by Kirby Linvill on 2/25/21.
//

#ifndef WEBSERVER_HTTP_MESSAGE_H
#define WEBSERVER_HTTP_MESSAGE_H

#include <string>

#include "content_type.h"


class HttpHeader {
public:
    HttpHeader(int status, std::string status_string, ContentType content_type, int content_length);
    operator std::string() const;

    int status;
    std::string status_string;
    ContentType content_type;
    int content_length;
};

class HttpMessage {
public:
    HttpMessage(int status, std::string status_string, ContentType content_type, std::string content);
    operator std::string() const;

    HttpHeader header;
    std::string content;
};


#endif //WEBSERVER_HTTP_MESSAGE_H
