#pragma once
#include "PL0/Core/Scanner.hpp"
#include "PL0/Utils/Word.hpp"

namespace PL0
{
class Lexer
{
public:
    Lexer(Scanner& scanner) : m_scanner(scanner)
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
    Scanner& m_scanner;
};
}  // namespace PL0
