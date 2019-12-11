#pragma once

#include "expr.h"

struct ASTPrinter : public Visitor {
    std::string text;

    const std::string& print(const Expr &expr);

    void visit(const Grouping &g) override;
    void visit(const Literal &l) override;
    void visit(const Unary &u) override;
    void visit(const Binary &b) override;
};

