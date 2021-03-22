#pragma once

#include <any>
#include <vector>
#include "interpreter.h"

struct LoxCallable {
    virtual ~LoxCallable() = default;

    // Max arity is 255
    virtual size_t arity() const = 0;

    virtual std::any call(Interpreter &interpreter, std::vector<std::any> &args) = 0;
};

// Native function to return the current time in seconds
struct Clock : LoxCallable {
    size_t arity() const override;

    std::any call(Interpreter &interpreter, std::vector<std::any> &args) override;
};

// A test function that adds the two arguments together for CI
struct CITestAdd : LoxCallable {
    size_t arity() const override;

    std::any call(Interpreter &interpreter, std::vector<std::any> &args) override;
};
