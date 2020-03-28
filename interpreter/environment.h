#pragma once

#include <any>
#include <memory>
#include <string>
#include <unordered_map>

class Environment {
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, std::any> values;

public:
    Environment(std::shared_ptr<Environment> &enclosing);

    Environment() = default;

    void define(const std::string &name, const std::any &val);

    void assign(const std::string &name, const std::any &val);

    std::any get(const std::string &name) const;
};
