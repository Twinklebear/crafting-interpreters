#pragma once

#include <string>
#include <vector>
#include "token.h"

struct Scanner {
    std::string source;
    std::vector<Token> tokens;

    // The scanner's location in the source
    int start = 0;
    int current = 0;
    int line = 1;

    Scanner() = default;
    Scanner(const std::string &source);

    const std::vector<Token> &scan_tokens();

private:
    void scan_token();

    // Add the token along with the currently spanned region of the source
    void add_token(TokenType type);

    // Add the token literal along with the currently spanned region of the source
    template <typename T>
    void add_token(TokenType type, const T &literal);

    char advance();

    bool match(char c);

    char peek() const;

    char peek_next() const;

    bool at_end() const;

    void scan_string();

    void scan_number();
};

template <typename T>
void Scanner::add_token(TokenType type, const T &literal)
{
    tokens.push_back(Token(type, source.substr(start, current - start), literal, line));
}

