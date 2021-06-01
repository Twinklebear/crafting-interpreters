#pragma once

#include <any>
#include <string>
#include "antlr4-runtime.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

// antlrcpp::Any doesn't properly handle storing std::string types,
// they have to be wrapped in a pointer
using StringPtr = std::shared_ptr<std::string>;

std::string get_file_content(const std::string &fname);

void error(const antlr4::Token *t, const std::string &msg);

std::string pretty_type_name(const antlrcpp::Any &t);

std::string pretty_type_name(const std::type_info &t);
