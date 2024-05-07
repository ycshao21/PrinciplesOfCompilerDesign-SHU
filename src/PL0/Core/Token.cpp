#include "PL0/Core/Token.hpp"
#include <format>

namespace PL0
{

std::string encode(const Token& token)
{
    switch (token.type) {
    case TokenType::Keyword: {
        return token.value + "sym";
    }
    case TokenType::Identifier: {
        return "ident";
    }
    case TokenType::Number: {
        return "number";
    }
    case TokenType::Operator: {
        if (token.value == "+") {
            return "plus";
        } else if (token.value == "-") {
            return "minus";
        } else if (token.value == "*") {
            return "times";
        } else if (token.value == "/") {
            return "slash";
        } else if (token.value == "=") {
            return "eql";
        } else if (token.value == "#") {
            return "neq";
        } else if (token.value == "<") {
            return "lss";
        } else if (token.value == "<=") {
            return "leq";
        } else if (token.value == ">") {
            return "gtr";
        } else if (token.value == ">=") {
            return "geq";
        } else if (token.value == ":=") {
            return "becomes";
        } else {
            throw std::runtime_error(std::format("Unknown operator: {}", token.value));
        }
    }
    case TokenType::Delimiter: {
        if (token.value == "(") {
            return "lparen";
        } else if (token.value == ")") {
            return "rparen";
        } else if (token.value == ",") {
            return "comma";
        } else if (token.value == ";") {
            return "semicolon";
        } else if (token.value == ".") {
            return "period";
        } else {
            throw std::runtime_error(std::format("Unknown delimiter: {}", token.value));
        }
    }
    }
    return "nul";
}

std::string translate2Symbol(const Token& token)
{
    switch (token.type) {
    case TokenType::Keyword: {
        return token.value;
    }
    case TokenType::Identifier: {
        return "id";
    }
    case TokenType::Number: {
        return "num";
    }
    case TokenType::Operator: {
        return token.value;
    }
    case TokenType::Delimiter: {
        return token.value;
    }
    }
    return "nul";
}

}  // namespace PL0