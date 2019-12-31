#include "parser.h"

ParseError::ParseError() : runtime_error("ParseError") {}

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse()
{
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!at_end()) {
        statements.push_back(statement());
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::statement()
{
    if (match({TokenType::PRINT})) {
        return print_statement();
    }
    return expression_statement();
}

std::shared_ptr<Stmt> Parser::print_statement()
{
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expected ; after print statement");
    return std::make_shared<Print>(value);
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expected ; after print statement");
    return std::make_shared<Expression>(value);
}

std::shared_ptr<Expr> Parser::expression()
{
    return equality();
}

std::shared_ptr<Expr> Parser::equality()
{
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        const Token &op = previous();
        auto right = comparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::comparison()
{
    auto expr = addition();

    while (match({TokenType::GREATER,
                  TokenType::GREATER_EQUAL,
                  TokenType::LESS,
                  TokenType::LESS_EQUAL})) {
        const Token &op = previous();
        auto right = addition();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::addition()
{
    auto expr = multiplication();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        const Token &op = previous();
        auto right = multiplication();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::multiplication()
{
    auto expr = unary();

    while (match({TokenType::STAR, TokenType::SLASH})) {
        const Token &op = previous();
        auto right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS})) {
        const Token &op = previous();
        auto right = unary();
        return std::make_shared<Unary>(op, right);
    }
    return primary();
}

std::shared_ptr<Expr> Parser::primary()
{
    if (match({TokenType::FALSE})) {
        return std::make_shared<Literal>(false);
    }
    if (match({TokenType::TRUE})) {
        return std::make_shared<Literal>(true);
    }
    if (match({TokenType::NIL})) {
        return std::make_shared<Literal>(std::any());
    }
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_shared<Literal>(previous().literal);
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return std::make_shared<Grouping>(expr);
    }

    error(peek(), "Expected expression");
    throw ParseError();
}

void Parser::synchronize()
{
    advance();

    while (!at_end()) {
        if (previous().type == TokenType::SEMICOLON) {
            return;
        }

        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        default:
            break;
        }
        advance();
    }
}

bool Parser::match(const std::vector<TokenType> &types)
{
    for (const auto &t : types) {
        if (check(t)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(const TokenType &t) const
{
    return !at_end() && peek().type == t;
}

bool Parser::at_end() const
{
    return peek().type == TokenType::END_OF_FILE;
}

const Token &Parser::advance()
{
    if (!at_end()) {
        ++current;
    }
    return previous();
}

const Token &Parser::consume(const TokenType &t, const std::string &error_message)
{
    if (check(t)) {
        return advance();
    }
    error(peek(), error_message);
    throw ParseError();
}

const Token &Parser::peek() const
{
    return tokens[current];
}

const Token &Parser::previous() const
{
    return tokens[current - 1];
}
