//
// Created by Kirby Linvill on 2/25/21.
//

#include "content_type.h"

#include <stdexcept>

#include "file_helpers.h"


// Throws invalid_argument exception
ContentType from_filename(std::string filename) {
    std::string extension = get_extension(filename);

    if (extension == "html" || extension == "htm")
            return ContentType::html;
    else if (extension == "txt")
            return ContentType::txt;
    else if (extension == "png")
            return ContentType::png;
    else if (extension == "gif")
            return ContentType::gif;
    else if (extension == "jpg")
            return ContentType::jpg;
    else if (extension == "css")
            return ContentType::css;
    else if (extension == "js")
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
