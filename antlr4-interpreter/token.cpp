#include "token.h"

Token::Token(TokenType type, int line) : type(type), line(line) {}

Token::Token(TokenType type, const std::string &lexeme, int line)
    : type(type), lexeme(lexeme), line(line)
{
}

std::string to_string(const TokenType &t)
{
    switch (t) {
    case TokenType::LEFT_PAREN:
        return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN:
        return "RIGHT_PAREN";
    case TokenType::LEFT_BRACE:
        return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE:
        return "RIGHT_BRACE";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::DOT:
        return "DOT";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::SLASH:
        return "SLASH";
    case TokenType::STAR:
        return "STAR";
    case TokenType::BANG:
        return "BANG";
    case TokenType::BANG_EQUAL:
        return "BANG_EQUAL";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::EQUAL_EQUAL:
        return "EQUAL_EQUAL";
    case TokenType::GREATER:
        return "GREATER";
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case TokenType::LESS:
        return "LESS";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::STRING:
        return "STRING";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::AND:
        return "AND";
    case TokenType::CLASS:
        return "CLASS";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::FALSE:
        return "FALSE";
    case TokenType::FUN:
        return "FUN";
    case TokenType::FOR:
        return "FOR";
    case TokenType::IF:
        return "IF";
    case TokenType::NIL:
        return "NIL";
    case TokenType::OR:
        return "OR";
    case TokenType::PRINT:
        return "PRINT";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::SUPER:
        return "SUPER";
    case TokenType::THIS:
        return "THIS";
    case TokenType::TRUE:
        return "TRUE";
    case TokenType::VAR:
        return "VAR";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::END_OF_FILE:
        return "END_OF_FILE";
    default:
        return "UNRECOGNIZED TOKEN!";
    }
}

std::ostream &operator<<(std::ostream &os, const TokenType &t)
{
    os << to_string(t);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Token &t)
{
    os << "[Token]: " << t.type << " " << t.lexeme << " ";
    if (t.literal.has_value()) {
        if (t.literal.type() == typeid(float)) {
            os << std::any_cast<float>(t.literal);
        } else if (t.literal.type() == typeid(std::string)) {
            os << std::any_cast<std::string>(t.literal);
        } else {
            os << "UNRECOGNIZED LITERAL!";
        }
        os << " ";
    }
    os << "l" << t.line;
    return os;
}

