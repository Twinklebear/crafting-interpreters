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
    void visit(const Call &c) override;
    void visit(const Logical &l) override;
    void visit(const Variable &v) override;
    void visit(const Assign &a) override;
    void visit(const Get &g) override;
    void visit(const Set &s) override;
};

struct ProgramPrinter : Stmt::Visitor {
    std::string text;

    const std::string &print(const std::vector<std::shared_ptr<Stmt>> &statements);

    void visit(const Block &b) override;
    void visit(const Expression &e) override;
    void visit(const If &f) override;
    void visit(const While &w) override;
    void visit(const Print &p) override;
    void visit(const Var &v) override;
    void visit(const Function &v) override;
    void visit(const Return &r) override;
    void visit(const Class &c) override;
};
