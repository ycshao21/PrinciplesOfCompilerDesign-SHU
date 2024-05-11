#pragma once
#include "PL0/Utils/Scanner.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>

namespace PL0
{
class Lexer
{
public:
    Lexer() = default;
    ~Lexer() = default;

    /**
     * @brief Convert the PL/0 source code into a sequence of tokens.
     * @param srcFile The path to the PL/0 source file.
     * @return The sequence of tokens.
     * @note All the invalid identifiers will be reported.
     */
    std::vector<Token> tokenize(const std::string& srcFile);

    /**
     * @brief Encode a token into a string.
     * @param token The token to be encoded.
     */
    static std::string encode(const Token& token) noexcept;

private:
    Token getKeywordOrIdentifier();
    Token getNumber();
    Token getOperator();
    Token getDelimiter();
    Token getUnknownSymbol();

private:
    std::unique_ptr<Scanner> m_scanner = nullptr;
};
}  // namespace PL0
