#pragma once
#include <any>
#include "token.h"
struct Visitor;
struct Expr {
    virtual void accept(Visitor &v) const = 0;
};
struct Unary : public Expr {
    Token op;
    std::shared_ptr<Expr> expr;
    Unary(Token op, std::shared_ptr<Expr> expr);
    void accept(Visitor &v) const override;
};
struct Literal : public Expr {
    std::any value;
    Literal(std::any value);
    void accept(Visitor &v) const override;
};
struct Binary : public Expr {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right);
    void accept(Visitor &v) const override;
};
struct Grouping : public Expr {
    std::shared_ptr<Expr> expr;
    Grouping(std::shared_ptr<Expr> expr);
    void accept(Visitor &v) const override;
};
struct Visitor {
    virtual void visit(const Unary &) = 0;
    virtual void visit(const Literal &) = 0;
    virtual void visit(const Binary &) = 0;
    virtual void visit(const Grouping &) = 0;
};
