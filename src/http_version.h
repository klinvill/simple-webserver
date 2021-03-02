//
// Created by Kirby Linvill on 3/2/21.
//

#ifndef WEBSERVER_HTTP_VERSION_H
#define WEBSERVER_HTTP_VERSION_H

#include <string>


enum class HttpVersionEnum {
    HTTP_1_0,   // HTTP/1.0
    HTTP_1_1,   // HTTP/1.1
};

class HttpVersion {
public:
    HttpVersion(const std::string& version);
    HttpVersion(HttpVersionEnum version);
    operator std::string() const;
    bool operator ==(const HttpVersion& rhs) const {
        return this->version == rhs.version;
    }

    HttpVersionEnum version;
};

#endif //WEBSERVER_HTTP_VERSION_H
