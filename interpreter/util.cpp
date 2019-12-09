#include "util.h"
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

std::string get_file_content(const std::string &fname)
{
    std::ifstream file{fname};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open " + fname);
    }
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

