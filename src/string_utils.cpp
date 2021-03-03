//
// Created by Kirby Linvill on 3/2/21.
//

#include "string_utils.h"

#include <algorithm>


std::string tolower(const std::string& text) {
    std::string copy(text);
    std::transform(copy.begin(), copy.end(), copy.begin(), [](char c) {return std::tolower(c);});
    return copy;
}
