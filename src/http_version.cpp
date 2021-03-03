//
// Created by Kirby Linvill on 3/2/21.
//

#include "http_version.h"

#include <stdexcept>


// Throws invalid_argument exception
HttpVersion::HttpVersion(const std::string& version) {
    if (version == "HTTP/1.0")
        this->version = HttpVersionEnum::HTTP_1_0;
    else if (version == "HTTP/1.1")
        this->version = HttpVersionEnum::HTTP_1_1;
    else
        throw std::invalid_argument(std::string("Unrecognized http version: ") + version);
}

HttpVersion::HttpVersion(HttpVersionEnum version) : version(version)
{}

HttpVersion::operator std::string() const {
    switch(this->version) {
        case HttpVersionEnum::HTTP_1_0:
            return "HTTP/1.0";
        case HttpVersionEnum::HTTP_1_1:
            return "HTTP/1.1";
    }
}
