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

    Environment(const Environment &e) = delete;
    Environment &operator=(const Environment &e) = delete;

    void define(const std::string &name, const std::any &val);

    void assign(const std::string &name, const std::any &val);

    void assign_at(const size_t depth, const std::string &name, const std::any &val);

    std::any get(const std::string &name) const;

    std::any get_at(const size_t depth, const std::string &name) const;

private:
    const Environment &ancestor(const size_t depth) const;

    Environment &ancestor(const size_t depth);
};
