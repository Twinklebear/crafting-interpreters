#pragma once

#include <string>

struct LoxClass {
    const std::string name;

    LoxClass(const std::string &name);

    const std::string &to_string() const;
};
