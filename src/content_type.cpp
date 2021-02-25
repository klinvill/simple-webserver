//
// Created by Kirby Linvill on 2/25/21.
//

#include "content_type.h"

#include <stdexcept>

#include "file_helpers.h"


// Throws invalid_argument exception
ContentType from_filename(std::string filename) {
    std::string extension = get_extension(filename);

    if (filename.compare(".html") == 0)
            return ContentType::html;
    else if (filename.compare(".txt") == 0)
            return ContentType::txt;
    else if (filename.compare(".png") == 0)
            return ContentType::png;
    else if (filename.compare(".gif") == 0)
            return ContentType::gif;
    else if (filename.compare(".jpg") == 0)
            return ContentType::jpg;
    else if (filename.compare(".css") == 0)
            return ContentType::css;
    else if (filename.compare(".js") == 0)
            return ContentType::js;
    else
        throw std::invalid_argument("Unrecognized file extension, cannot determine ContentType");
}


// returns the string representation for the http Content-Type
std::string to_string(ContentType type) {
    switch(type) {
        case ContentType::html:
            return "text/html";
        case ContentType::txt:
            return "text/txt";
        case ContentType::png:
            return "image/png";
        case ContentType::gif:
            return "image/gif";
        case ContentType::jpg:
            return "image/jpg";
        case ContentType::css:
            return "text/css";
        case ContentType::js:
            return "application/javascript";
    }
}
