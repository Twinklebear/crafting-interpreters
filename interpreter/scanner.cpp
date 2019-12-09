#include "scanner.h"
#include "util.h"

Scanner::Scanner(const std::string &source) : source(source) {}

const std::vector<Token> &Scanner::scan_tokens()
{
    tokens.clear();

    while (!at_end()) {
        // Advance to the next lexeme and scan it
        start = current;
        scan_token();
    }
    tokens.push_back(Token(TokenType::END_OF_FILE, line));
    return tokens;
}

void Scanner::scan_token()
{
    char c = advance();
    switch (c) {
    case '(':
        add_token(TokenType::LEFT_PAREN);
        break;
    case ')':
        add_token(TokenType::RIGHT_PAREN);
        break;
    case '{':
        add_token(TokenType::LEFT_BRACE);
        break;
    case '}':
        add_token(TokenType::RIGHT_BRACE);
        break;
    case ',':
        add_token(TokenType::COMMA);
        break;
    case '.':
        add_token(TokenType::DOT);
        break;
    case '-':
        add_token(TokenType::MINUS);
        break;
    case '+':
        add_token(TokenType::PLUS);
        break;
    case ';':
        add_token(TokenType::SEMICOLON);
        break;
    case '*':
        add_token(TokenType::STAR);
        break;
    case '!':
        add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '>':
        add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '<':
        add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '/':
        // If it's a comment, advance to the next line to skip it
        if (match('/')) {
            while (peek() != '\n' && !at_end()) {
                advance();
            }
        } else {
            add_token(TokenType::SLASH);
        }
        break;
    case ' ':
    case '\t':
    case '\r':
        break;
    case '\n':
        ++line;
        break;
    default:
        error(line, "Unrecognized character '" + std::string(1, c) + "'");
        break;
    }
}

void Scanner::add_token(TokenType type)
{
    tokens.push_back(Token(type, source.substr(start, current - start), line));
}

char Scanner::advance()
{
    return source[current++];
}

bool Scanner::match(char c)
{
    if (at_end() || source[current] != c) {
        return false;
    }
    ++current;
    return true;
}

char Scanner::peek() const
{
    if (at_end()) {
        return '\0';
    }
    return source[current];
}

bool Scanner::at_end() const
{
    return current >= source.length();
}

