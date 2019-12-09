#pragma once

#include <string>

extern bool had_error;

std::string get_file_content(const std::string &fname);

void report(int line, const std::string &where, const std::string &msg);

void error(int line, const std::string &msg);

