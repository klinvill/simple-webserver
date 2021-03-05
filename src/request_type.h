//
// Data structure for HTTP request type (e.g. GET)
//

#ifndef WEBSERVER_REQUEST_TYPE_H
#define WEBSERVER_REQUEST_TYPE_H

#include <string>


enum class RequestTypeEnum {
    GET,
    POST,
};

class RequestType {
public:
    // Parses request type from a string
    // e.g. "GET" -> RequestTypeEnum::GET
    // Throws invalid_argument exception
    RequestType(const std::string& type);
    RequestType(RequestTypeEnum type);

    // Converts request type to a string
    // e.g. RequestTypeEnum::GET -> "GET"
    operator std::string() const;

    bool operator ==(const RequestType& rhs) const {
        return this->type == rhs.type;
    }

    RequestTypeEnum type;
};

#endif //WEBSERVER_REQUEST_TYPE_H
