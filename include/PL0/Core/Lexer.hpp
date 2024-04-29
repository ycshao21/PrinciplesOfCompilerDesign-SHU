#pragma once
#include "PL0/Core/Scanner.hpp"
#include "PL0/Core/Word.hpp"

namespace PL0
{
class Lexer
{
public:
    Lexer(const std::string& srcFile) : m_scanner(srcFile)
    {
    }
    ~Lexer() = default;
    Word getNextWord();

private:
    Word getNumber();
    Word getOperator();
    Word getDelimiter();
    Word getKeywordOrIdentifier();
    Word getUnknownSymbol();

private:
    Scanner m_scanner;
};
}  // namespace PL0
