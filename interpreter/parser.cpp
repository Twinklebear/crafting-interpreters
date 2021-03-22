#include "parser.h"
#include <iostream>

ParseError::ParseError() : runtime_error("ParseError") {}

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse()
{
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::declaration()
{
    try {
        if (match({TokenType::VAR})) {
            return var_declaration();
        }

        return statement();
    } catch (const std::runtime_error &e) {
        std::cout << "interpreter error: " << e.what() << "\n";
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::var_declaration()
{
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name in declaration");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expected ; after variable declaration");
    return std::make_shared<Var>(name, initializer);
}

std::shared_ptr<Stmt> Parser::statement()
{
    if (match({TokenType::IF})) {
        return if_statement();
    }
    if (match({TokenType::WHILE})) {
        return while_statement();
    }
    if (match({TokenType::FOR})) {
        return for_statement();
    }
    if (match({TokenType::PRINT})) {
        return print_statement();
    }
    if (match({TokenType::LEFT_BRACE})) {
        return block_statement();
    }
    return expression_statement();
}

std::shared_ptr<Stmt> Parser::if_statement()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition");

    auto then_branch = statement();
    std::shared_ptr<Stmt> else_branch;
    if (match({TokenType::ELSE})) {
        else_branch = statement();
    }

    return std::make_shared<If>(condition, then_branch, else_branch);
}

std::shared_ptr<Stmt> Parser::print_statement()
{
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expected ; after print statement");
    return std::make_shared<Print>(value);
}

std::shared_ptr<Stmt> Parser::while_statement()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition");

    auto body = statement();

    return std::make_shared<While>(condition, body);
}

std::shared_ptr<Stmt> Parser::for_statement()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    std::shared_ptr<Stmt> initializer;
    if (!match({TokenType::SEMICOLON})) {
        if (match({TokenType::VAR})) {
            initializer = var_declaration();
        } else {
            initializer = expression_statement();
        }
    }

    std::shared_ptr<Expr> condition;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after for loop condition");

    std::shared_ptr<Expr> increment;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses");

    auto body = statement();

    // Desugar the for statement into a while loop for execution
    if (increment) {
        body = std::make_shared<Block>(
            std::vector<std::shared_ptr<Stmt>>{body, std::make_shared<Expression>(increment)});
    }

    if (!condition) {
        condition = std::make_shared<Literal>(true);
    }
    body = std::make_shared<While>(condition, body);

    if (initializer) {
        body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>>{initializer, body});
    }

    return body;
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expected ; after print statement");
    return std::make_shared<Expression>(value);
}

std::shared_ptr<Stmt> Parser::block_statement()
{
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !at_end()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' closing block");
    return std::make_shared<Block>(statements);
}

std::shared_ptr<Expr> Parser::expression()
{
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment()
{
    auto expr = or_expr();

    if (match({TokenType::EQUAL})) {
        const Token &equals = previous();
        auto value = assignment();

        if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            return std::make_shared<Assign>(var->name, value);
        }
        error(equals, "Expected expression");
    }
    return expr;
}

std::shared_ptr<Expr> Parser::or_expr()
{
    auto expr = and_expr();

    while (match({TokenType::OR})) {
        const Token &op = previous();
        auto right = and_expr();
        expr = std::make_shared<Logical>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::and_expr()
{
    auto expr = equality();

    while (match({TokenType::AND})) {
        const Token &op = previous();
        auto right = equality();
        expr = std::make_shared<Logical>(expr, op, right);
    }
    return expr;
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
    return call();
}

std::shared_ptr<Expr> Parser::call()
{
    auto expr = primary();
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finish_call(expr);
        } else {
            // Note: applicable later in the book
            break;
        }
    }
    return expr;
}

std::shared_ptr<Expr> Parser::finish_call(std::shared_ptr<Expr> &callee)
{
    std::vector<std::shared_ptr<Expr>> args;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (args.size() >= 255) {
                error(peek(), "Functions cannot take more than 255 arguments");
            }
            args.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments");

    return std::make_shared<Call>(callee, paren, args);
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
    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<Variable>(previous());
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
