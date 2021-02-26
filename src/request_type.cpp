//
// Created by Kirby Linvill on 2/25/21.
//

#include "request_type.h"

// Throws invalid_argument exception
RequestType::RequestType(const std::string& type) {
    if (type == "GET")
        this->type = RequestTypeEnum::GET;
    else if (type == "POST")
        this->type = RequestTypeEnum::POST;
    else
        throw std::invalid_argument(std::string("Unrecognized request type: ") + type);
}

RequestType::RequestType(RequestTypeEnum type) : type(type)
{}

RequestType::operator std::string() const {
    switch(this->type) {
        case RequestTypeEnum::GET:
            return "GET";
        case RequestTypeEnum::POST:
            return "POST";
    }
}
