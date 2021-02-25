//
// Created by Kirby Linvill on 2/25/21.
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

ContentType from_filename(std::string filename);
std::string to_string(ContentType type);

#endif //WEBSERVER_CONTENT_TYPE_H
