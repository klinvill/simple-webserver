//
// Miscellaneous file helpers
//

#include "file_helpers.h"


std::string get_extension(std::string filename) {
    size_t extension_start = filename.find_last_of(".");
    // no dot found in name or dot is the last character
    if (extension_start == std::string::npos || extension_start + 1 >= filename.length())
        return std::string();

    return filename.substr(extension_start+1);
}

