#include "token.h"

Token::Token(TokenType type, int line) : type(type), line(line) {}

Token::Token(TokenType type, const std::string &lexeme, int line)
    : type(type), lexeme(lexeme), line(line)
{
}

std::ostream &operator<<(std::ostream &os, const TokenType &t)
{
    switch (t) {
    case TokenType::LEFT_PAREN:
        os << "LEFT_PAREN";
        break;
    case TokenType::RIGHT_PAREN:
        os << "RIGHT_PAREN";
        break;
    case TokenType::LEFT_BRACE:
        os << "LEFT_BRACE";
        break;
    case TokenType::RIGHT_BRACE:
        os << "RIGHT_BRACE";
        break;
    case TokenType::COMMA:
        os << "COMMA";
        break;
    case TokenType::DOT:
        os << "DOT";
        break;
    case TokenType::MINUS:
        os << "MINUS";
        break;
    case TokenType::PLUS:
        os << "PLUS";
        break;
    case TokenType::SEMICOLON:
        os << "SEMICOLON";
        break;
    case TokenType::SLASH:
        os << "SLASH";
        break;
    case TokenType::STAR:
        os << "STAR";
        break;
    case TokenType::BANG:
        os << "BANG";
        break;
    case TokenType::BANG_EQUAL:
        os << "BANG_EQUAL";
        break;
    case TokenType::EQUAL:
        os << "EQUAL";
        break;
    case TokenType::EQUAL_EQUAL:
        os << "EQUAL_EQUAL";
        break;
    case TokenType::GREATER:
        os << "GREATER";
        break;
    case TokenType::GREATER_EQUAL:
        os << "GREATER_EQUAL";
        break;
    case TokenType::LESS:
        os << "LESS";
        break;
    case TokenType::LESS_EQUAL:
        os << "LESS_EQUAL";
        break;
    case TokenType::IDENTIFIER:
        os << "IDENTIFIER";
        break;
    case TokenType::STRING:
        os << "STRING";
        break;
    case TokenType::NUMBER:
        os << "NUMBER";
        break;
    case TokenType::AND:
        os << "AND";
        break;
    case TokenType::CLASS:
        os << "CLASS";
        break;
    case TokenType::ELSE:
        os << "ELSE";
        break;
    case TokenType::FALSE:
        os << "FALSE";
        break;
    case TokenType::FUN:
        os << "FUN";
        break;
    case TokenType::FOR:
        os << "FOR";
        break;
    case TokenType::IF:
        os << "IF";
        break;
    case TokenType::NIL:
        os << "NIL";
        break;
    case TokenType::OR:
        os << "OR";
        break;
    case TokenType::PRINT:
        os << "PRINT";
        break;
    case TokenType::RETURN:
        os << "RETURN";
        break;
    case TokenType::SUPER:
        os << "SUPER";
        break;
    case TokenType::THIS:
        os << "THIS";
        break;
    case TokenType::TRUE:
        os << "TRUE";
        break;
    case TokenType::VAR:
        os << "VAR";
        break;
    case TokenType::WHILE:
        os << "WHILE";
        break;
    case TokenType::END_OF_FILE:
        os << "END_OF_FILE";
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Token &t)
{
    os << "[Token]: " << t.type << " " << t.lexeme << " ";
    if (t.literal.has_value()) {
        os << t.literal.type().name();
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

