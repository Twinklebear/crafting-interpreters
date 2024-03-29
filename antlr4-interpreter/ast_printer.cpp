#include "ast_printer.h"
#include <iostream>
#include "antlr4-runtime.h"

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
    text += "(" + std::to_string(u.op->getType()) + " '" + u.op->getText() + "' ";
    u.expr->accept(*this);
    text += ")";
}

void ASTPrinter::visit(const Binary &b)
{
    text += "(" + std::to_string(b.op->getType()) + " '" + b.op->getText() + "' ";
    b.left->accept(*this);
    text += " ";
    b.right->accept(*this);
    text += ")";
}

void ASTPrinter::visit(const Call &c)
{
    text += "(CALL ";
    c.callee->accept(*this);
    text += ")";
}

void ASTPrinter::visit(const Logical &l)
{
    text += "(" + std::to_string(l.op->getType()) + " '" + l.op->getText() + "' ";
    l.left->accept(*this);
    text += " ";
    l.right->accept(*this);
    text += ")";
}

void ASTPrinter::visit(const Variable &v)
{
    text += "(variable '" + v.name->getText() + "')";
}

void ASTPrinter::visit(const Assign &a)
{
    text += "(assignment '" + a.name->getText() + "' = ";
    a.value->accept(*this);
    text += ")";
}

void ASTPrinter::visit(const Get &g)
{
    text += "(get property '" + g.name->getText() + "' of ";
    g.object->accept(*this);
    text += ")";
}

void ASTPrinter::visit(const Set &s)
{
    text += "(set property '" + s.name->getText() + "' of ";
    s.object->accept(*this);
    text += " = ";
    s.value->accept(*this);
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
    ProgramPrinter then_printer;
    text += then_printer.print({f.then_branch});
    if (f.else_branch) {
        ProgramPrinter else_printer;
        text += "ELSE: " + else_printer.print({f.else_branch});
    }
    text += "}";
}

void ProgramPrinter::visit(const While &w)
{
    text += "{WHILE Stmt, cond: ";
    ASTPrinter ast_printer;
    text += ast_printer.print(*w.condition) + "\nLOOP: ";
    // TODO: would be nice for readability to add indentation here
    ProgramPrinter then_printer;
    text += then_printer.print({w.body}) + "}";
}

void ProgramPrinter::visit(const Print &p)
{
    text += "{PRINT Stmt ";
    ASTPrinter ast_printer;
    text += ast_printer.print(*p.expr) + "}";
}

void ProgramPrinter::visit(const Var &v)
{
    text += "{VAR Stmt '" + v.token->getText() + "'";
    if (v.initializer) {
        ASTPrinter ast_printer;
        text += " = " + ast_printer.print(*v.initializer);
    }
    text += "}";
}

void ProgramPrinter::visit(const Function &v)
{
    text += "{FUNCTION Stmt '" + v.name->getText() + "' (";
    for (size_t i = 0; i < v.params.size(); ++i) {
        text += v.params[i]->getText();
        if (i + 1 < v.params.size()) {
            text += ", ";
        }
    }
    text += ")\n BODY:\n";
    ProgramPrinter printer;
    printer.print({v.body});
    text += printer.text + "}";
}

void ProgramPrinter::visit(const Return &r)
{
    text += "{RETURN Stmt '";
    ASTPrinter ast_printer;
    if (r.value) {
        text += ast_printer.print(*r.value);
    }
    text += "}";
}

void ProgramPrinter::visit(const Class &c)
{
    text += "{CLASS Stmt '" + c.name->getText() + "'\n";

    for (const auto &m : c.methods) {
        visit(*m);
    }
    text += "\n}";
}
