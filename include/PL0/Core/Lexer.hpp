#pragma once
#include "PL0/Core/Scanner.hpp"
#include "PL0/Core/Token.hpp"
#include <vector>
#include <memory>

namespace PL0
{
class Lexer
{
public:
    Lexer() = default;
    ~Lexer() = default;

    std::vector<Token> tokenize(const std::string& srcFile);

private:
    Token getNextToken();

    Token getNumber();
    Token getOperator();
    Token getDelimiter();
    Token getKeywordOrIdentifier();
    Token getUnknownSymbol();

private:
    std::unique_ptr<Scanner> m_scanner = nullptr;
};
}  // namespace PL0
