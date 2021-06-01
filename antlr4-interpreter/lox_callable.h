#pragma once

#include <any>
#include <vector>
#include "antlr4-runtime.h"
#include "interpreter.h"

struct LoxCallable {
    virtual ~LoxCallable() = default;

    // Max arity is 255
    virtual size_t arity() = 0;

    virtual antlrcpp::Any call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args) = 0;

    virtual std::string to_string() const = 0;
};

// Native function to return the current time in seconds
struct Clock : LoxCallable {
    size_t arity() override;

    antlrcpp::Any call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args) override;

    std::string to_string() const override;
};

// A test function that adds the two arguments together for CI
struct CITestAdd : LoxCallable {
    size_t arity() override;

    antlrcpp::Any call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args) override;

    std::string to_string() const override;
};

// A function defined in Lox
struct LoxFunction : LoxCallable {
    LoxParser::FunctionContext *declaration;
    std::shared_ptr<Environment> closure;

    LoxFunction(LoxParser::FunctionContext *declaration,
                const std::shared_ptr<Environment> &closure);

    size_t arity() override;

    antlrcpp::Any call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args) override;

    std::string to_string() const override;
};
