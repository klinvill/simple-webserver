//
// Created by Kirby Linvill on 2/25/21.
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
    RequestType(const std::string& type);
    RequestType(RequestTypeEnum type);
    operator std::string() const;
    bool operator ==(const RequestType& rhs) const {
        return this->type == rhs.type;
    }

    RequestTypeEnum type;
};

#endif //WEBSERVER_REQUEST_TYPE_H
