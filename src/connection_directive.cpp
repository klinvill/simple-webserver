//
// Created by Kirby Linvill on 3/2/21.
//

#include "connection_directive.h"

#include "string_utils.h"


ConnectionDirective::ConnectionDirective() {
    this->directive = ConnectionDirectiveEnum::EMPTY;
}

// Throws invalid_argument exception
ConnectionDirective::ConnectionDirective(const std::string& directive) {
    std::string lowercase_directive = tolower(directive);
    if (lowercase_directive == "keep-alive")
        this->directive = ConnectionDirectiveEnum::KEEP_ALIVE;
    else if (lowercase_directive == "close")
        this->directive = ConnectionDirectiveEnum::CLOSE;
    else
        throw std::invalid_argument(std::string("Unrecognized connection directive: ") + directive);
}

ConnectionDirective::ConnectionDirective(ConnectionDirectiveEnum directive) : directive(directive)
{}

ConnectionDirective::operator std::string() const {
    switch(this->directive) {
        case ConnectionDirectiveEnum::KEEP_ALIVE:
            return "Keep-alive";
        case ConnectionDirectiveEnum::CLOSE:
            return "Close";
        case ConnectionDirectiveEnum::EMPTY:
            return "";
    }
}
