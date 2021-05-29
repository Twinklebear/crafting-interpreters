#pragma once

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include "antlr4-runtime.h"

class Environment {
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, antlrcpp::Any> values;

public:
    Environment(std::shared_ptr<Environment> &enclosing);

    Environment() = default;

    Environment(const Environment &e) = delete;
    Environment &operator=(const Environment &e) = delete;

    void define(const std::string &name, const antlrcpp::Any &val);

    void assign(const std::string &name, const antlrcpp::Any &val);

    void assign_at(const size_t depth, const std::string &name, const antlrcpp::Any &val);

    antlrcpp::Any get(const std::string &name) const;

    antlrcpp::Any get_at(const size_t depth, const std::string &name) const;

private:
    const Environment &ancestor(const size_t depth) const;

    Environment &ancestor(const size_t depth);
};
