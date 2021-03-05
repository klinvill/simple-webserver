//
// Miscellaneous file helpers
//

#include "file_helpers.h"

#include <sstream>
#include <sys/stat.h>


std::string get_extension(const std::string& filename) {
    size_t extension_start = filename.find_last_of('.');
    // no dot found in name or dot is the last character
    if (extension_start == std::string::npos || extension_start + 1 >= filename.length())
        return std::string();

    return filename.substr(extension_start+1);
}

std::string join_filepath(const std::string& dir, const std::string& file) {
    std::stringstream joined_path;
    joined_path << dir;

    if (dir.back() != '/')
        joined_path << '/';

    if (file.front() == '/')
        joined_path << file.substr(1);
    else
        joined_path << file;

    return joined_path.str();
}

bool is_file(const std::string& filepath) {
    struct stat stat_info{};
    stat(filepath.c_str(), &stat_info);

    return S_ISREG(stat_info.st_mode);
}

