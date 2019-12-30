#pragma once

#include <typeinfo>
#include <vector>
#include "expr.h"

struct InterpreterError {
    Token token;
    std::string message;

    InterpreterError(const Token &t, const std::string &msg);
};

struct Interpreter : public Visitor {
    std::any result;

    const std::any &evaluate(const Expr &expr);

    void visit(const Grouping &g) override;
    void visit(const Literal &l) override;
    void visit(const Unary &u) override;
    void visit(const Binary &b) override;

private:
    bool check_type(const std::any &val, const std::vector<std::string> &valid_types);
};

