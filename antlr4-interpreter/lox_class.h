#pragma once

#include <string>
#include <unordered_map>
#include "antlr4-runtime.h"
#include "lox_callable.h"

struct LoxClass : LoxCallable {
    const std::string name;

    LoxClass(const std::string &name);

    size_t arity() const override;

    antlrcpp::Any call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args) override;

    std::string to_string() const override;
};

struct LoxInstance {
    const LoxClass &lox_class;
    std::unordered_map<std::string, antlrcpp::Any> fields;

    LoxInstance(const LoxClass &lox_class);

    std::string to_string() const;

    antlrcpp::Any get(const antlr4::Token *name);

    void set(const antlr4::Token *name, const antlrcpp::Any &value);
};
