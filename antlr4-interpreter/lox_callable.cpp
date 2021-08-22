#include "lox_callable.h"
#include <chrono>
#include <iostream>
#include "util.h"

size_t Clock::arity()
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

size_t CITestAdd::arity()
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
    } else if (left.is<StringPtr>() && right.is<StringPtr>()) {
        auto l = left.as<StringPtr>();
        auto r = right.as<StringPtr>();
        result = std::make_shared<std::string>(*l + *r);
    } else {
        throw InterpreterError(
            nullptr, "Invalid arguments to _ci_test_add: Must be two numbers or strings");
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
    size_t n = 0;
    if (declaration->parameters()) {
        for (const auto &p : declaration->parameters()->children) {
            // TODO: building AST will fix this annoying issue where
            // tokens I don't care about are here in the tree
            if (p->getText() == ",") {
                continue;
            }
            ++n;
        }
    }
    return n;
}

antlrcpp::Any LoxFunction::call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args)
{
    // Create a new environment for the function and set up its local variables
    // with the argument values
    auto environment = std::make_shared<Environment>(closure);
    EnvironmentContext env_ctx(&interpreter.environment, environment);

    if (declaration->parameters()) {
        auto formal_params = declaration->parameters()->IDENTIFIER();
        for (size_t i = 0; i < formal_params.size(); ++i) {
            environment->define(formal_params[i]->getText(), args[i]);
        }
    }

    try {
        interpreter.visit(declaration->block());
    } catch (const std::shared_ptr<ReturnControlFlow> &ret) {
        return ret->value;
    }
    return antlrcpp::Any();
}

std::string LoxFunction::to_string() const
{
    return "<fn " + declaration->IDENTIFIER()->getText() + ">";
}
