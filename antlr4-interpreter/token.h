#pragma once

#include <any>
#include <ostream>
#include <string>

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::any literal;
    int line;

    Token() = default;

    Token(TokenType type, int line);

    Token(TokenType type, const std::string &lexeme, int line);

    template <typename T>
    Token(TokenType type, const std::string &lexeme, const T &literal, int line);
};

template <typename T>
Token::Token(TokenType type, const std::string &lexeme, const T &literal, int line)
    : type(type), lexeme(lexeme), literal(literal), line(line)
{
}

std::string to_string(const TokenType &t);

std::ostream& operator<<(std::ostream &os, const TokenType &t);

std::ostream &operator<<(std::ostream &os, const Token &t);

