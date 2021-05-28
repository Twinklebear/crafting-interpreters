#pragma once

#include <any>
#include <string>
#include "antlr4-runtime.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

extern bool had_error;

std::string get_file_content(const std::string &fname);

void report(int line, const std::string &where, const std::string &msg);

void error(int line, const std::string &msg);

void error(const antlr4::Token *t, const std::string &msg);

std::string pretty_type_name(const std::any &t);

std::string pretty_type_name(const std::type_info &t);
