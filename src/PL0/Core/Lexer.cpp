#include "PL0/Core/Lexer.hpp"
#include "PL0/Utils/Reporter.hpp"

#include <algorithm>
#include <string>

namespace PL0
{
Lexer::Lexer(const std::string& srcFile) : m_scanner(srcFile)
{
}

Token Lexer::getNextToken()
{
    m_scanner.skipSpaceAndComments();
    char c = m_scanner.get();

    if (isAlpha(c)) {
        return getKeywordOrIdentifier();
    } else if (isDigit(c)) {
        return getNumber();
    } else if (isDelimiter(c)) {
        return getDelimiter();
    } else if (isOperatorChar(c)) {
        return getOperator();
    } else if (c == EOF) {
        return {TokenType::EndOfFile, ""};
    } else {
        return getUnknownSymbol();
    }
}

Token Lexer::getNumber()
{
    Token token{TokenType::Number, m_scanner.getUntil(isDigit)};
    if (isAlpha(m_scanner.get())) {
        token.type = TokenType::Invalid;
        token.value += m_scanner.getUntil(isAlphaOrDigit);
        // Reporter::error("Invalid identifier: {}", token.value);
        Reporter::error(std::format("Invalid identifier: {}", token.value));
    }
    return token;
}

Token Lexer::getDelimiter()
{
    Token token = {TokenType::Delimiter, m_scanner.getAsStr()};
    m_scanner.forward();
    return token;
}

Token Lexer::getOperator()
{
    Token token{TokenType::Operator, m_scanner.getAsStr()};
    m_scanner.forward();
    if (token.value == "<" || token.value == ">") {
        if (m_scanner.get() == '=') {  // >=, <=
            token.value += '=';
            m_scanner.forward();
        }
    } else if (token.value == ":") {
        if (m_scanner.get() == '=') {  // :=
            token.value += '=';
            m_scanner.forward();
        } else {
            token.type = TokenType::Invalid;
            // Reporter::error("Invalid operator", token.value);
            Reporter::error(std::format("Invalid operator: {}", token.value));
        }
    }
    return token;
}

Token Lexer::getKeywordOrIdentifier()
{
    Token token;
    token.value = m_scanner.getUntil(isAlphaOrDigit);
    std::ranges::transform(token.value, token.value.begin(), ::tolower);
    if (std::ranges::find(KEYWORDS, token.value) != KEYWORDS.end()) {
        token.type = TokenType::Keyword;
    } else {
        token.type = TokenType::Identifier;
    }
    return token;
}

Token Lexer::getUnknownSymbol()
{
    Token token{TokenType::Invalid, m_scanner.getAsStr()};
    // Reporter::error("Unknown symbol", token.value);
    Reporter::error(std::format("Unknown symbol: {}", token.value));
    m_scanner.forward();
    return token;
}

}  // namespace PL0