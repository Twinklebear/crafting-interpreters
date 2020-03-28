#pragma once

#include <any>
#include <string>
#include <unordered_map>

class Environment {
    std::unordered_map<std::string, std::any> values;

public:
    void define(const std::string &name, const std::any &val);

    void assign(const std::string &name, const std::any &val);

    std::any get(const std::string &name) const;
};
