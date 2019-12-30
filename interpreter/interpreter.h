#pragma once

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <vector>
#include "expr.h"

struct InterpreterError {
    Token token;
    std::string message;

    InterpreterError(const Token &t, const std::string &msg);
};

struct Interpreter : public Visitor {
    std::any result;

    Interpreter();

    const std::any &evaluate(const Expr &expr);

    void visit(const Grouping &g) override;
    void visit(const Literal &l) override;
    void visit(const Unary &u) override;
    void visit(const Binary &b) override;

private:
    std::type_index float_id, string_id, bool_id, nil_id;
    std::unordered_map<std::type_index, std::string> type_names;

    // Check if the type is one of the specified valid types, if not throws an
    // InterpreterError
    void check_type(const std::any &val,
                    const std::vector<std::type_index> &valid_types,
                    const Token &t);

    // Check if the two anys have the same type, if not throws an InterpreterError
    void check_same_type(const std::any &a, const std::any &b, const Token &t);
};
