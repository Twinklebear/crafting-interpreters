#include "lox_class.h"

LoxClass::LoxClass(const std::string &name) : name(name) {}

const std::string &LoxClass::to_string() const
{
    return name;
}
