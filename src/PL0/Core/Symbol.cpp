#include "PL0/Core/Symbol.hpp"

namespace PL0
{
Symbol translate2Symbol(const Token& token)
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

Symbol translate2Symbol(const std::string& tokenValue)
{
    if (std::isdigit(tokenValue[0])) {
        return "num";
    } else if (std::isalpha(tokenValue[0])) {
        return "id";
    } else {
        return tokenValue;
    }
}

Element::Element(const Symbol& sym, bool synthesized)
    : symbol(sym)
{
    if (sym == ENDSYM) {
        type = SymbolType::ENDSYM;
    } else if (std::isdigit(sym[0])) {
        type = SymbolType::ACTION;
    } else if (std::isupper(sym[0])) {
        if (synthesized) {
            type = SymbolType::SYNTHESIZED;
        } else {
            type = SymbolType::NON_TERMINAL;
        }
    } else {
        type = SymbolType::TERMINAL;
    }
}

} // namespace PL0