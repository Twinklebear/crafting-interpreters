#include "lox_callable.h"
#include <chrono>
#include <iostream>

size_t Clock::arity() const
{
    return 0;
}

antlrcpp::Any Clock::call(Interpreter &, std::vector<antlrcpp::Any> &)
{
    using namespace std::chrono;
    const auto now = steady_clock::now();
    const float millis = duration_cast<milliseconds>(now.time_since_epoch()).count();
    return millis / 1000.f;
}

std::string Clock::to_string() const
{
    return "<fn clock>";
}

size_t CITestAdd::arity() const
{
    return 2;
}

antlrcpp::Any CITestAdd::call(Interpreter &, std::vector<antlrcpp::Any> &args)
{
    auto left = args[0];
    auto right = args[1];
    antlrcpp::Any result;
    if (left.is<float>() && right.is<float>()) {
        result = left.as<float>() + right.as<float>();
    } else if (left.is<std::string>() && right.is<std::string>()) {
        result = left.as<std::string>() + right.as<std::string>();
    } else {
        throw InterpreterError(
            nullptr, "Invalid arguments to _ci_test_add: Must be two numbers of strings");
    }
    return result;
}

std::string CITestAdd::to_string() const
{
    return "<fn _ci_test_add>";
}

LoxFunction::LoxFunction(LoxParser::FunctionContext *declaration,
                         const std::shared_ptr<Environment> &closure)
    : declaration(declaration), closure(closure)
{
}

size_t LoxFunction::arity()
{
    if (!declaration->parameters()) {
        return 0;
    }
    return declaration->parameters()->children.size();
}

antlrcpp::Any LoxFunction::call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args)
{
    // Create a new environment for the function and set up its local variables
    // with the argument values
    auto environment = std::make_shared<Environment>(closure);
    if (declaration->parameters()) {
        auto formal_params = declaration->parameters()->IDENTIFIER();
        for (size_t i = 0; i < formal_params.size(); ++i) {
            environment->define(formal_params[i]->getText(), args[i]);
        }
    }

    try {
        interpreter.visit_with_environment(declaration->block(), environment);
    } catch (const std::shared_ptr<ReturnControlFlow> &ret) {
        return ret->value;
    }
    return antlrcpp::Any();
}

std::string LoxFunction::to_string() const
{
    return "<fn " + declaration->IDENTIFIER()->getText() + ">";
}
