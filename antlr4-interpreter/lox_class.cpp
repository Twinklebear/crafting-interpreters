#include "lox_class.h"

LoxClass::LoxClass(const std::string &name) : name(name) {}

size_t LoxClass::arity()
{
    return 0;
}

antlrcpp::Any LoxClass::call(Interpreter &interpreter, std::vector<antlrcpp::Any> &args)
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

antlrcpp::Any LoxInstance::get(const antlr4::Token *name)
{
    auto fnd = fields.find(name->getText());
    if (fnd != fields.end()) {
        return fnd->second;
    }
    throw InterpreterError(name, "Undefined property '" + name->getText() + "'");
}

void LoxInstance::set(const antlr4::Token *name, const antlrcpp::Any &value)
{
    fields[name->getText()] = value;
}
