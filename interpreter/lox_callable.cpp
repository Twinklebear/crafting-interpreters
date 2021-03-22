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
