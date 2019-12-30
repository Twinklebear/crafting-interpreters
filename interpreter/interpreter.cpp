#include "interpreter.h"
#include <iostream>
#include "util.h"

InterpreterError::InterpreterError(const Token &t, const std::string &msg)
    : token(t), message(msg)
{
}

const std::any &Interpreter::evaluate(const Expr &expr)
{
    result = std::any();
    expr.accept(*this);
    return result;
}

void Interpreter::visit(const Grouping &g)
{
    result = evaluate(*g.expr);
}

void Interpreter::visit(const Literal &l)
{
    result = l.value;
}

void Interpreter::visit(const Unary &u)
{
    std::any right = evaluate(*u.expr);
    switch (u.op.type) {
    case TokenType::MINUS:
        if (!check_type(right, {typeid(float).name()})) {
            throw InterpreterError(
                u.op, "Expected float operand but got " + pretty_type_name(right.type()));
        }
        result = -std::any_cast<float>(right);
        break;
    default:
        break;
    }
}

void Interpreter::visit(const Binary &b)
{
    std::any left = evaluate(*b.left);
    std::any right = evaluate(*b.right);

    switch (b.op.type) {
    case TokenType::PLUS:
        if (!check_type(left, {typeid(float).name(), typeid(std::string).name()})) {
            throw InterpreterError(
                b.op,
                "Expected float or string operand but got " + pretty_type_name(left.type()));
        }
        if (left.type() != right.type()) {
            throw InterpreterError(b.op,
                                   "Expected " + pretty_type_name(left.type()) +
                                       " operand but got " + pretty_type_name(right.type()));
        }
        if (left.type() == typeid(float) && right.type() == typeid(float)) {
            result = std::any_cast<float>(left) + std::any_cast<float>(right);
        } else if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
            result = std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
        }
    default:
        break;
    }
}

bool Interpreter::check_type(const std::any &val, const std::vector<std::string> &valid_types)
{
    for (const auto &t : valid_types) {
        if ((val.has_value() && val.type().name() == t) ||
            (!val.has_value() && t == typeid(void).name())) {
            return true;
        }
    }
    return false;
}
