#include "util.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

bool had_error = false;

std::string get_file_content(const std::string &fname)
{
    std::ifstream file{fname};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open " + fname);
    }
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

void report(int line, const std::string &where, const std::string &msg)
{
    std::cerr << "[line " << line << "] Error " << where << ": " << msg << "\n";
}

void error(int line, const std::string &msg)
{
    had_error = true;
    report(line, "", msg);
}

void error(const Token &t, const std::string &msg)
{
    had_error = true;
    if (t.type == TokenType::END_OF_FILE) {
        report(t.line, " at end of file", msg);
    } else {
        report(t.line, " at '" + t.lexeme + "'", msg);
    }
}
