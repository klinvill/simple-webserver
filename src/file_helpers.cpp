//
// Created by Kirby Linvill on 2/25/21.
//

#include "file_helpers.h"

// Given a filename (e.g. `test.txt`) returns the extension (e.g. `txt`). This returns an empty string if no extension
// can be determined.
//
// Would ideally use boost::filesystem, but it's not installed on the lab machines
std::string get_extension(std::string filename) {
    size_t extension_start = filename.find_last_of(".");
    // no dot found in name or dot is the last character
    if (extension_start == std::string::npos || extension_start + 1 >= filename.length())
        return std::string();

    return filename.substr(extension_start+1);
}

