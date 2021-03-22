#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "environment.h"
#include "expr.h"

struct InterpreterError {
    Token token;
    std::string message;

    InterpreterError(const Token &t, const std::string &msg);
};

struct Interpreter : Expr::Visitor, Stmt::Visitor {
    std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(globals);
    std::any result;

    Interpreter();

    void evaluate(const std::vector<std::shared_ptr<Stmt>> &statements);

    const std::any &evaluate(const Expr &expr);

    void execute_block(const std::vector<std::shared_ptr<Stmt>> &statements,
                       std::shared_ptr<Environment> &env);

    void visit(const Grouping &g) override;
    void visit(const Literal &l) override;
    void visit(const Unary &u) override;
    void visit(const Binary &b) override;
    void visit(const Call &c) override;
    void visit(const Logical &l) override;
    void visit(const Variable &v) override;
    void visit(const Assign &a) override;

    void visit(const Block &b) override;
    void visit(const Expression &e) override;
    void visit(const If &f) override;
    void visit(const While &w) override;
    void visit(const Print &p) override;
    void visit(const Var &v) override;
    void visit(const Function &f) override;

private:
    std::type_index float_id, string_id, bool_id, nil_id, callable_id;
    std::unordered_map<std::type_index, std::string> type_names;

    // Check if the type is one of the specified valid types, if not throws an
    // InterpreterError
    void check_type(const std::any &val,
                    const std::vector<std::type_index> &valid_types,
                    const Token &t);

    // Check if the two anys have the same type, if not throws an InterpreterError
    void check_same_type(const std::any &a, const std::any &b, const Token &t) const;

    bool is_true(const std::any &x) const;

    bool is_equal(const std::any &a, const std::any &b) const;
};
