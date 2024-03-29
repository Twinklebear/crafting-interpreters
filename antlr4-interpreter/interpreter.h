#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "antlr4-common.h"
#include "antlr4-runtime.h"
#include "environment.h"
#include "expr.h"

struct InterpreterError {
    const antlr4::Token *token;
    std::string message;

    InterpreterError(const antlr4::Token *t, const std::string &msg);
};

struct ReturnControlFlow {
    std::any value;

    ReturnControlFlow(const std::any &value);

    ReturnControlFlow(const ReturnControlFlow &r) = default;

    ReturnControlFlow() = default;
};

struct Interpreter : Expr::Visitor, Stmt::Visitor {
    std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    std::shared_ptr<Environment> environment = globals;
    // Track the depth each variable expresion is resolved to
    // So couldn't this just be "Variable*"?
    // NOTE: The pointers all refer to objects held in std::shared_ptr, though
    // with how the visitor pattern works here the shared ptr is not directly
    // accessible since visit is called by the object itself.
    // Maybe clox introduces a better design here, or just uses raw pointers throughout?
    std::unordered_map<const Expr *, size_t> locals;
    std::any result;

    Interpreter();

    void evaluate(const std::vector<std::shared_ptr<Stmt>> &statements);

    const std::any &evaluate(const Expr &expr);

    void execute_block(const std::vector<std::shared_ptr<Stmt>> &statements,
                       std::shared_ptr<Environment> &env);

    void resolve(const Expr &expr, size_t depth);

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
    std::type_index float_id, string_id, bool_id, nil_id, callable_id;
    std::unordered_map<std::type_index, std::string> type_names;

    // Check if the type is one of the specified valid types, if not throws an
    // InterpreterError
    void check_type(const std::any &val,
                    const std::vector<std::type_index> &valid_types,
                    const antlr4::Token *t);

    // Check if the two anys have the same type, if not throws an InterpreterError
    void check_same_type(const std::any &a, const std::any &b, const antlr4::Token *t) const;

    bool is_true(const std::any &x) const;

    bool is_equal(const std::any &a, const std::any &b) const;

    std::any lookup_variable(const antlr4::Token *token, const Expr &expr) const;
};
