//
// Miscellaneous file helpers
//

#ifndef WEBSERVER_FILE_HELPERS_H
#define WEBSERVER_FILE_HELPERS_H

#include <string>

// Given a filename (e.g. `test.txt`) returns the extension (e.g. `txt`). This returns an empty string if no extension
// can be determined.
//
// Would ideally use boost::filesystem, but it's not installed on the lab machines
std::string get_extension(const std::string& filename);

// helper function to return a unix-style path containing both dir and file
// e.g. join_filepath("www/images/", "/cool/cool.png") will return "www/images/cool/cool.png"
std::string join_filepath(const std::string& dir, const std::string& file);

// Checks if the item specified by a path is a file
bool is_file(const std::string& filepath);

#endif //WEBSERVER_FILE_HELPERS_H
