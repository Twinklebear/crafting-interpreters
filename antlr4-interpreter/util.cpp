#include "util.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include "antlr4-runtime.h"

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

void error(const antlr4::Token *t, const std::string &msg)
{
    had_error = true;
    if (t) {
        report(t->getLine(), " at '" + t->getText() + "'", msg);
    } else {
        report(-1, "unknown/generated token location", msg);
    }
}

std::string pretty_type_name(const antlrcpp::Any &t)
{
    if (t.is<float>()) {
        return "float";
    }
    if (t.is<StringPtr>()) {
        return "string";
    }
    if (t.is<bool>()) {
        return "bool";
    }
    if (t.isNull()) {
        return "nil";
    }
    return std::string("UNHANDLED TYPE") + antlrcpp::toString(t);
}

std::string pretty_type_name(const std::type_info &t)
{
    if (t == typeid(float)) {
        return "float";
    }
    if (t == typeid(StringPtr)) {
        return "string";
    }
    if (t == typeid(bool)) {
        return "bool";
    }
    if (t == typeid(void)) {
        return "nil";
    }
    return std::string("UNHANDLED TYPE ") + t.name();
}
