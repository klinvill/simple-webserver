//
// Data structure for Connection Keep-Alive header
//

#ifndef WEBSERVER_CONNECTION_DIRECTIVE_H
#define WEBSERVER_CONNECTION_DIRECTIVE_H

#include <string>


enum class ConnectionDirectiveEnum {
    KEEP_ALIVE,
    CLOSE,
    // ideally we could just use std::optional, but that's only available in C++17
    EMPTY,  // indicates no connection header was provided
};

class ConnectionDirective {
public:
    ConnectionDirective();
    // parses a connection header from the string version
    // e.g. "Connection: keep-alive" -> ConnectionDirectiveEnum::KEEP_ALIVE
    // Throws invalid_argument exception
    ConnectionDirective(const std::string& directive);
    ConnectionDirective(ConnectionDirectiveEnum directive);

    // returns the string representation of a connection header
    // e.g. ConnectionDirectiveEnum::KEEP_ALIVE -> "Connection: Keep-alive"
    operator std::string() const;

    bool operator ==(const ConnectionDirective& rhs) const {
        return this->directive == rhs.directive;
    }

    ConnectionDirectiveEnum directive;
};

#endif //WEBSERVER_CONNECTION_DIRECTIVE_H
