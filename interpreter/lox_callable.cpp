#include "lox_callable.h"
#include <chrono>

size_t Clock::arity() const
{
    return 0;
}

std::any Clock::call(Interpreter &, std::vector<std::any> &)
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

std::any CITestAdd::call(Interpreter &, std::vector<std::any> &args)
{
    auto left = args[0];
    auto right = args[1];
    std::any result;
    if (left.type() == typeid(float) && right.type() == typeid(float)) {
        result = std::any_cast<float>(left) + std::any_cast<float>(right);
    } else if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
        result = std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
    } else {
        throw InterpreterError(
            Token(), "Invalid arguments to _ci_test_add: Must be two numbers of strings");
    }
    return result;
}

std::string CITestAdd::to_string() const
{
    return "<fn _ci_test_add>";
}

LoxFunction::LoxFunction(const Function &declaration) : declaration(declaration) {}

size_t LoxFunction::arity() const
{
    return declaration.params.size();
}

std::any LoxFunction::call(Interpreter &interpreter, std::vector<std::any> &args)
{
    // Create a new environment for the function and set up its local variables
    // with the argument values
    std::shared_ptr<Environment> environment =
        std::make_shared<Environment>(interpreter.globals);
    for (size_t i = 0; i < declaration.params.size(); ++i) {
        environment->define(declaration.params[i].lexeme, args[i]);
    }

    interpreter.execute_block({declaration.body}, environment);
    return std::any();
}

std::string LoxFunction::to_string() const
{
    return "<fn " + declaration.name.lexeme + ">";
}
