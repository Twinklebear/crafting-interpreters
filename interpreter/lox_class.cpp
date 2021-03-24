#include "lox_class.h"

LoxClass::LoxClass(const std::string &name) : name(name) {}

size_t LoxClass::arity() const
{
    return 0;
}

std::any LoxClass::call(Interpreter &interpreter, std::vector<std::any> &args)
{
    return std::make_shared<LoxInstance>(*this);
}

std::string LoxClass::to_string() const
{
    return name;
}

LoxInstance::LoxInstance(const LoxClass &lc) : lox_class(lc) {}

std::string LoxInstance::to_string() const
{
    return lox_class.name + " instance";
}

std::any LoxInstance::get(const Token &name)
{
    auto fnd = fields.find(name.lexeme);
    if (fnd != fields.end()) {
        return fnd->second;
    }
    throw InterpreterError(name, "Undefined property '" + name.lexeme + "'");
}

void LoxInstance::set(const Token &name, const std::any &value)
{
    fields[name.lexeme] = value;
}
