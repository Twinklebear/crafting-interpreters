#include "lox_class.h"

LoxClass::LoxClass(const std::string &name) : name(name) {}

size_t LoxClass::arity() const
{
    return 0;
}

std::any LoxClass::call(Interpreter &, std::vector<std::any> &)
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

std::any LoxInstance::get(const antlr4::Token *name)
{
    auto fnd = fields.find(name->getText());
    if (fnd != fields.end()) {
        return fnd->second;
    }
    throw InterpreterError(name, "Undefined property '" + name->getText() + "'");
}

void LoxInstance::set(const antlr4::Token *name, const std::any &value)
{
    fields[name->getText()] = value;
}
