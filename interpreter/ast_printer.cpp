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

void ASTPrinter::visit(const Logical &l)
{
    text += "(" + to_string(l.op.type) + " '" + l.op.lexeme + "' ";
    l.left->accept(*this);
    text += " ";
    l.right->accept(*this);
    text += ")";
}

void ASTPrinter::visit(const Variable &v)
{
    text += "(variable '" + v.name.lexeme + "')";
}

void ASTPrinter::visit(const Assign &a)
{
    text += "(assignment '" + a.name.lexeme + "' = ";
    a.value->accept(*this);
    text += ")";
}

const std::string &ProgramPrinter::print(const std::vector<std::shared_ptr<Stmt>> &statements)
{
    text = "";
    for (const auto &st : statements) {
        st->accept(*this);
        text += "\n";
    }
    return text;
}

void ProgramPrinter::visit(const Block &b)
{
    text += "{BLOCK Stmt\n";
    ProgramPrinter printer;
    text += printer.print(b.statements);
    text += "}";
}

void ProgramPrinter::visit(const Expression &e)
{
    text += "{EXPRESSION Stmt ";
    ASTPrinter ast_printer;
    text += ast_printer.print(*e.expr) + "}";
}

void ProgramPrinter::visit(const If &f)
{
    text += "{IF Stmt, cond: ";
    ASTPrinter ast_printer;
    text += ast_printer.print(*f.condition) + "\nTHEN: ";
    // TODO: would be nice for readability to add indentation here
    ProgramPrinter then_printer;
    text += then_printer.print({f.then_branch});
    if (f.else_branch) {
        ProgramPrinter else_printer;
        text += "ELSE: " + else_printer.print({f.else_branch});
    }
    text += "}";
}

void ProgramPrinter::visit(const Print &p)
{
    text += "{PRINT Stmt ";
    ASTPrinter ast_printer;
    text += ast_printer.print(*p.expr) + "}";
}

void ProgramPrinter::visit(const Var &v)
{
    text += "{VAR Stmt '" + v.token.lexeme + "'";
    if (v.initializer) {
        ASTPrinter ast_printer;
        text += " = " + ast_printer.print(*v.initializer);
    }
    text += "}";
}
