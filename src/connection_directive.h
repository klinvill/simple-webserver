//
// Created by Kirby Linvill on 3/2/21.
//

#ifndef WEBSERVER_CONNECTION_DIRECTIVE_H
#define WEBSERVER_CONNECTION_DIRECTIVE_H

#include <string>


enum class ConnectionDirectiveEnum {
    KEEP_ALIVE,
    CLOSE,
    EMPTY,  // indicates no connection header was provided
};

class ConnectionDirective {
public:
    ConnectionDirective();
    ConnectionDirective(const std::string& directive);
    ConnectionDirective(ConnectionDirectiveEnum directive);
    operator std::string() const;
    bool operator ==(const ConnectionDirective& rhs) const {
        return this->directive == rhs.directive;
    }

    ConnectionDirectiveEnum directive;
};

#endif //WEBSERVER_CONNECTION_DIRECTIVE_H
