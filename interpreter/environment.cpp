#include "environment.h"
#include <iostream>
#include <stdexcept>

Environment::Environment(std::shared_ptr<Environment> &enclosing) : enclosing(enclosing) {}

void Environment::define(const std::string &name, const std::any &val)
{
    values[name] = val;
}

void Environment::assign(const std::string &name, const std::any &val)
{
    auto fnd = values.find(name);
    if (fnd != values.end()) {
        fnd->second = val;
    } else if (enclosing) {
        enclosing->assign(name, val);
    } else {
        throw std::runtime_error("Undefined variable '" + name + "'");
    }
}

std::any Environment::get(const std::string &name) const
{
    auto fnd = values.find(name);
    if (fnd != values.end()) {
        return fnd->second;
    } else if (enclosing) {
        return enclosing->get(name);
    }
    throw std::runtime_error("Undefined variable '" + name + "'");
}
