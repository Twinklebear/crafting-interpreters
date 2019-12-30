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

Interpreter::Interpreter()
    : float_id(std::type_index(typeid(float))),
      string_id(std::type_index(typeid(std::string))),
      bool_id(std::type_index(typeid(bool))),
      nil_id(std::type_index(typeid(void)))
{
    type_names[float_id] = pretty_type_name(typeid(float));
    type_names[string_id] = pretty_type_name(typeid(std::string));
    type_names[bool_id] = pretty_type_name(typeid(bool));
    type_names[nil_id] = pretty_type_name(typeid(void));
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
        check_type(right, {float_id}, u.op);
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
        check_type(left, {float_id, string_id}, b.op);
        check_same_type(left, right, b.op);

        if (left.type() == typeid(float) && right.type() == typeid(float)) {
            result = std::any_cast<float>(left) + std::any_cast<float>(right);
        } else if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
            result = std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
        }
        break;
    case TokenType::MINUS:
        check_type(left, {float_id, string_id}, b.op);
        check_same_type(left, right, b.op);

        result = std::any_cast<float>(left) - std::any_cast<float>(right);
        break;
    default:
        break;
    }
}

void Interpreter::check_type(const std::any &val,
                             const std::vector<std::type_index> &valid_types,
                             const Token &t)
{
    for (const auto &t : valid_types) {
        if (std::type_index(val.type()) == t) {
            return;
        }
    }
    std::string error_msg = "Expected one of {";
    for (size_t i = 0; i < valid_types.size(); ++i) {
        error_msg += type_names[valid_types[i]];
        if (i + 1 < valid_types.size()) {
            error_msg += ", ";
        }
    }
    error_msg += "} but got " + pretty_type_name(val);
    throw InterpreterError(t, error_msg);
}

void Interpreter::check_same_type(const std::any &a, const std::any &b, const Token &t)
{
    if (a.type() != b.type()) {
        throw InterpreterError(
            t, "Expected " + pretty_type_name(a) + " but got " + pretty_type_name(b));
    }
}
