//
// Created by Kirby Linvill on 2/25/21.
//

#include "http_message.h"

#include <sstream>

#define SEPARATOR "\r\n"
#define HTTP_VERSION "1.1"

HttpHeader::HttpHeader(int status, std::string status_string, ContentType content_type, int content_length)
    : status(status), status_string(status_string), content_type(content_type), content_length(content_length)
{}

HttpHeader::operator std::string() const {
    std::stringstream buf;
    buf = std::stringstream("HTTP/") << HTTP_VERSION << " " << this->status << " " << this->status_string << SEPARATOR
        << "Content-Type: " << to_string(this->content_type) << SEPARATOR
        << "Content-Length: " << this->content_length << SEPARATOR;

    return buf.str();
}

HttpMessage::HttpMessage(int status, std::string status_string, ContentType content_type, std::string content)
    : content(content), header(status, status_string, content_type, content.length())
{}

HttpMessage::operator std::string() const {
    std::stringstream buf;
    buf = std::stringstream(std::string(header)) << SEPARATOR
            << this->content << SEPARATOR;
    return buf.str();
}
