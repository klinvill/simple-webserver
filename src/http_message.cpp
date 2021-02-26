//
// Created by Kirby Linvill on 2/25/21.
//

#include "http_message.h"

#include <sstream>

#define FIELD_SEPARATOR " "
#define KEY_VALUE_SEPARATOR ": "
#define SEPARATOR "\r\n"
#define HTTP_VERSION "1.1"

HttpResponseHeader::HttpResponseHeader(int status, std::string status_string, ContentType content_type, int content_length)
    : status(status), status_string(status_string), content_type(content_type), content_length(content_length)
{}

HttpResponseHeader::operator std::string() const {
    std::stringstream buf;
    buf = std::stringstream() << "HTTP/" << HTTP_VERSION << " " << this->status << " " << this->status_string << SEPARATOR
        << "Content-Type: " << to_string(this->content_type) << SEPARATOR
        << "Content-Length: " << this->content_length << SEPARATOR;

    return buf.str();
}


HttpResponseMessage::HttpResponseMessage(int status, std::string status_string, ContentType content_type, std::string content)
    : content(content), header(status, status_string, content_type, content.length())
{}

HttpResponseMessage::operator std::string() const {
    std::stringstream buf;
    buf = std::stringstream() << std::string(this->header) << SEPARATOR
            << this->content << SEPARATOR;
    return buf.str();
}


// static
// extracts the next field for parsing
//
// offset will be incremented by the number of bytes parsed to parse the type
std::string HttpRequestHeader::get_next_field(const std::string& data, unsigned long& offset) {
    int length = strcspn(&data.c_str()[offset], FIELD_SEPARATOR);
    std::string field = data.substr(offset, length);
    offset += length + strlen(FIELD_SEPARATOR);
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
HttpRequestHeader::HttpRequestHeader(const std::string& data) : type("GET") {
    unsigned long offset = 0;
    this->type = RequestType(this->get_next_field(data, offset));
    this->resource = this->get_next_field(data, offset);

    // We only care about the HTTP request type and resource, so we ignore any other information on the first line
    unsigned long headers_start = data.find(SEPARATOR) + strlen(SEPARATOR);
    unsigned long headers_end = data.find(std::string(SEPARATOR) + std::string(SEPARATOR));

    // If no Content-Length header is specified, default to 0
    this->content_length = 0;

    assert(headers_start >= offset);
    offset = headers_start;
    while(offset < headers_end) {
        KeyValue header = this->get_next_key_value(data, offset);
        // we only care about the Content-Length header, so we can just ignore the other headers
        if (header.key == "Content-Length") {
            this->content_length = std::stoi(header.value);
            // TODO: should probably use unsigned type
            assert(this->content_length > 0);
        }
    }
}


HttpRequestMessage::HttpRequestMessage(const std::string& data) : header(data) {
    unsigned long content_start = data.find(std::string(SEPARATOR) + std::string(SEPARATOR)) + 2 * strlen(SEPARATOR);
    this->content = data.substr(content_start, this->header.content_length);
}
