#pragma once
#include "PL0/Core/Token.hpp"
#include "PL0/Core/Scanner.hpp"

namespace PL0
{
class Lexer
{
public:
    Lexer(const std::string& srcFile);
    ~Lexer() = default;
    Token getNextToken();

private:
    Token getNumber();
    Token getOperator();
    Token getDelimiter();
    Token getKeywordOrIdentifier();
    Token getUnknownSymbol();

private:
    Scanner m_scanner;
};
}  // namespace PL0
