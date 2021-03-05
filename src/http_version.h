//
// Data structure for HTTP version
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
    // Parses HTTP version from a string
    // e.g. "HTTP/1.1" -> HttpVersionEnum::HTTP_1_1
    // Throws invalid_argument exception
    HttpVersion(const std::string& version);
    HttpVersion(HttpVersionEnum version);

    // Returns string representation of HTTP version
    // e.g. HttpVersionEnum::HTTP_1_1 -> "HTTP/1.1"
    operator std::string() const;
    bool operator ==(const HttpVersion& rhs) const {
        return this->version == rhs.version;
    }

    HttpVersionEnum version;
};

#endif //WEBSERVER_HTTP_VERSION_H
