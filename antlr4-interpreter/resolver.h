#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "antlr4-runtime.h"
#include "environment.h"
#include "expr.h"
#include "interpreter.h"

enum class FunctionType { NONE, FUNCTION };

struct VariableStatus {
    bool defined = false;
    bool read = false;
};

struct Resolver : Expr::Visitor, Stmt::Visitor {
    // Treated as a stack, but we need to access scopes by index as well
    // when resolving variables
    std::vector<std::unordered_map<std::string, VariableStatus>> scopes;
    FunctionType current_function = FunctionType::NONE;

    Interpreter &interpreter;

    Resolver(Interpreter &interpreter);

    void resolve(const std::vector<std::shared_ptr<Stmt>> &statements);

    // Visitors for expressions
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

    // Visitors for statements
    void visit(const Block &b) override;
    void visit(const Expression &e) override;
    void visit(const If &f) override;
    void visit(const While &w) override;
    void visit(const Print &p) override;
    void visit(const Var &v) override;
    void visit(const Function &f) override;
    void visit(const Return &r) override;
    void visit(const Class &c) override;

private:
    void begin_scope();

    void end_scope();

    void resolve(const std::shared_ptr<Stmt> &statement);
    void resolve(const std::shared_ptr<Expr> &expr);

    void declare(const antlr4::Token *name);
    void define(const antlr4::Token *name);

    void resolve_local(const Expr &expr, const antlr4::Token *name);

    void resolve_function(const Function &f, const FunctionType type);
};
