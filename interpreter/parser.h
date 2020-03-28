#pragma once

#include <any>
#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "expr.h"
#include "token.h"
#include "util.h"

struct ParseError : public std::runtime_error {
    ParseError();
};

struct Parser {
    std::vector<Token> tokens;
    int current = 0;

    Parser() = default;
    Parser(const std::vector<Token> &tokens);

    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::shared_ptr<Stmt> declaration();

    std::shared_ptr<Stmt> var_declaration();

    std::shared_ptr<Stmt> statement();

    std::shared_ptr<Stmt> print_statement();

    std::shared_ptr<Stmt> expression_statement();

    std::shared_ptr<Stmt> block_statement();

    std::shared_ptr<Expr> expression();

    std::shared_ptr<Expr> assignment();

    std::shared_ptr<Expr> equality();

    std::shared_ptr<Expr> comparison();

    std::shared_ptr<Expr> addition();

    std::shared_ptr<Expr> multiplication();

    std::shared_ptr<Expr> unary();

    std::shared_ptr<Expr> primary();

    void synchronize();

    bool match(const std::vector<TokenType> &types);

    bool check(const TokenType &t) const;

    bool at_end() const;

    const Token &advance();

    const Token &consume(const TokenType &t, const std::string &error_message);

    const Token &peek() const;

    const Token &previous() const;
};
