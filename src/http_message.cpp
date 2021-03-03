//
// Created by Kirby Linvill on 2/25/21.
//

#include "http_message.h"

#include <sstream>
#include <cstring>
#include <cassert>


#define FIELD_SEPARATORS " \r"
#define KEY_VALUE_SEPARATOR ": "
#define SEPARATOR "\r\n"

HttpResponseHeader::HttpResponseHeader(int status, std::string status_string, ContentType content_type,
                                       int content_length, HttpVersion version,
                                       ConnectionDirective connection_directive)
    : status(status), status_string(status_string), content_type(content_type), content_length(content_length),
    version(version), connection_directive(connection_directive)
{}

HttpResponseHeader::operator std::string() const {
    std::stringstream buf;
    buf << std::string(this->version) << " " << this->status << " " << this->status_string << SEPARATOR
        << "Content-Type: " << to_string(this->content_type) << SEPARATOR
        << "Content-Length: " << this->content_length << SEPARATOR;

    if (this->connection_directive.directive != ConnectionDirectiveEnum::EMPTY)
        buf << "Connection: " << std::string(this->connection_directive) << SEPARATOR;

    return buf.str();
}


HttpResponseMessage::HttpResponseMessage(int status, std::string status_string, ContentType content_type,
                                         std::string content, HttpVersion version,
                                         ConnectionDirective connection_directive)
    : content(content), header(status, status_string, content_type, content.length(), version, connection_directive)
{}

HttpResponseMessage::operator std::string() const {
    std::stringstream buf;
    buf << std::string(this->header) << SEPARATOR
        << this->content << SEPARATOR;
    return buf.str();
}


// static
// extracts the next field for parsing
//
// offset will be incremented by the number of bytes parsed to parse the type
std::string HttpRequestHeader::get_next_field(const std::string& data, unsigned long& offset) {
    int length = strcspn(&data.c_str()[offset], FIELD_SEPARATORS);
    std::string field = data.substr(offset, length);
    offset += length + 1;
    return field;
}

//static
// extracts the next key value pair in a HTTP header
//
// offset will be incremented by the number of bytes parsed to parse the type
KeyValue HttpRequestHeader::get_next_key_value(const std::string& data, unsigned long& offset) {
    unsigned long key_length = data.find(KEY_VALUE_SEPARATOR, offset) - offset;
    unsigned long value_offset = offset + key_length + strlen(KEY_VALUE_SEPARATOR);
    unsigned long value_length = data.find(SEPARATOR, value_offset) - value_offset;
    KeyValue result = {.key=data.substr(offset, key_length), .value=data.substr(value_offset, value_length)};
    offset = value_offset + value_length + strlen(SEPARATOR);
    return result;
}


// TODO: currently I use a GET type as a placeholder until the data can be parsed, should avoid instantiating an object
//  I just throw away
HttpRequestHeader::HttpRequestHeader(const std::string& data) : type("GET"), version(HttpVersionEnum::HTTP_1_1) {
    unsigned long offset = 0;
    this->type = RequestType(this->get_next_field(data, offset));
    this->resource = this->get_next_field(data, offset);
    this->version = HttpVersion(this->get_next_field(data, offset));

    // We care about the HTTP request type, resource, and version, ignoring any other information on the first line
    unsigned long headers_start = data.find(SEPARATOR) + strlen(SEPARATOR);
    unsigned long headers_end = data.find(std::string(SEPARATOR) + std::string(SEPARATOR));

    // If no Content-Length header is specified, default to 0
    this->content_length = 0;
    // If no Connection header is specified, we use the default EMPTY value
    this->connection_directive = ConnectionDirective();

    assert(headers_start >= offset);
    offset = headers_start;
    while(offset < headers_end) {
        KeyValue header = this->get_next_key_value(data, offset);
        // we only care about the Content-Length and Connection headers, so we can just ignore the other headers
        if (header.key == "Content-Length") {
            this->content_length = std::stoi(header.value);
            // TODO: should probably use unsigned type
            assert(this->content_length > 0);
        } else if (header.key == "Connection") {
            this->connection_directive = ConnectionDirective(header.value);
        }
    }
}


HttpRequestMessage::HttpRequestMessage(const std::string& data) : header(data) {
    unsigned long content_start = data.find(std::string(SEPARATOR) + std::string(SEPARATOR)) + 2 * strlen(SEPARATOR);
    this->content = data.substr(content_start, this->header.content_length);
}
