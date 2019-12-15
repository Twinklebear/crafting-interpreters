#include "ast_printer.h"
#include <iostream>

const std::string &ASTPrinter::print(const Expr &expr)
{
    text = "";
    expr.accept(*this);
    return text;
}

void ASTPrinter::visit(const Grouping &g)
{
    text += "(group ";
    g.expr->accept(*this);
    text += ")";
}
void ASTPrinter::visit(const Literal &l)
{
    if (l.value.has_value()) {
        if (l.value.type() == typeid(float)) {
            text += std::to_string(std::any_cast<float>(l.value));
        } else if (l.value.type() == typeid(std::string)) {
            text += std::any_cast<std::string>(l.value);
        } else if (l.value.type() == typeid(bool)) {
            text += std::any_cast<bool>(l.value) ? "true" : "false";
        }
    } else {
        text += "nil";
    }
}
void ASTPrinter::visit(const Unary &u)
{
    text += "(" + to_string(u.op.type) + " '" + u.op.lexeme + "' ";
    u.expr->accept(*this);
    text += ")";
}
void ASTPrinter::visit(const Binary &b)
{
    text += "(" + to_string(b.op.type) + " '" + b.op.lexeme + "' ";
    b.left->accept(*this);
    text += " ";
    b.right->accept(*this);
    text += ")";
}

