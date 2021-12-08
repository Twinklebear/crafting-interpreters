#pragma once

#include <string>
#include <unordered_map>
#include "antlr4-runtime.h"
#include "lox_callable.h"

struct LoxClass : LoxCallable {
    const std::string name;

    LoxClass(const std::string &name);

    size_t arity() const override;

    std::any call(Interpreter &interpreter, std::vector<std::any> &args) override;

    std::string to_string() const override;
};

struct LoxInstance {
    const LoxClass &lox_class;
    std::unordered_map<std::string, std::any> fields;

    LoxInstance(const LoxClass &lox_class);

    std::string to_string() const;

    std::any get(const antlr4::Token *name);

    void set(const antlr4::Token *name, const std::any &value);
};
