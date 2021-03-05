//
// Data structure and methods for Content-Type header
//

#ifndef WEBSERVER_CONTENT_TYPE_H
#define WEBSERVER_CONTENT_TYPE_H

#include <string>

enum class ContentType {
    html,
    txt,
    png,
    gif,
    jpg,
    css,
    js,
};

// constructs the relevant Content-Type header using the name of the file
// e.g. "index.html" -> ContentType::html
// Throws invalid_argument exception
ContentType from_filename(std::string filename);

// returns a string representation of the Content-Type header
// e.g. ContentType::html -> "text/html"
std::string to_string(ContentType type);

#endif //WEBSERVER_CONTENT_TYPE_H
