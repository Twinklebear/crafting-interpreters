#pragma once

#include <memory>
#include <vector>
#include "expr.h"

struct ASTPrinter : Expr::Visitor {
    std::string text;

    const std::string &print(const Expr &expr);

    void visit(const Grouping &g) override;
    void visit(const Literal &l) override;
    void visit(const Unary &u) override;
    void visit(const Binary &b) override;
};

struct ProgramPrinter : Stmt::Visitor {
    std::string text;

    const std::string &print(const std::vector<std::shared_ptr<Stmt>> &statements);

    void visit(const Expression &e) override;
    void visit(const Print &p) override;
};
