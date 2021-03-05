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
std::string get_extension(std::string filename);

#endif //WEBSERVER_FILE_HELPERS_H
