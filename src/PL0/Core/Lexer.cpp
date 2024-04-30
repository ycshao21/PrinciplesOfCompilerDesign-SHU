#include "PL0/Core/Lexer.hpp"
#include "PL0/Utils/Reporter.hpp"

#include <algorithm>
#include <string>

namespace PL0
{
std::vector<Token> Lexer::tokenize(const std::string& srcFile)
{
    m_scanner = std::make_unique<Scanner>(srcFile);

    std::vector<Token> tokens;

    while (true) {
        m_scanner->skipSpaceAndComments();
        char c = m_scanner->get();

        if (c == EOF) {
            break;
        }

        Token token;
        if (isAlpha(c)) {
            token = getKeywordOrIdentifier();
        } else if (isDigit(c)) {
            token = getNumber();
        } else if (isDelimiter(c)) {
            token = getDelimiter();
        } else if (isOperatorChar(c)) {
            token = getOperator();
        } else {
            token = getUnknownSymbol();
        }
        tokens.push_back(token);
    }

    m_scanner.reset();
    return tokens;
}

Token Lexer::getNumber()
{
    Token token{TokenType::Number, m_scanner->getUntil(isDigit)};
    if (isAlpha(m_scanner->get())) {
        token.type = TokenType::Invalid;
        token.value += m_scanner->getUntil(isAlphaOrDigit);
        // Reporter::error("Invalid identifier: {}", token.value);
        Reporter::error(std::format("Invalid identifier: {}", token.value));
    }
    return token;
}

Token Lexer::getDelimiter()
{
    Token token = {TokenType::Delimiter, m_scanner->getAsStr()};
    m_scanner->forward();
    return token;
}

Token Lexer::getOperator()
{
    Token token{TokenType::Operator, m_scanner->getAsStr()};
    m_scanner->forward();
    if (token.value == "<" || token.value == ">") {
        if (m_scanner->get() == '=') {  // >=, <=
            token.value += '=';
            m_scanner->forward();
        }
    } else if (token.value == ":") {
        if (m_scanner->get() == '=') {  // :=
            token.value += '=';
            m_scanner->forward();
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
    token.value = m_scanner->getUntil(isAlphaOrDigit);
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
    Token token{TokenType::Invalid, m_scanner->getAsStr()};
    // Reporter::error("Unknown symbol", token.value);
    Reporter::error(std::format("Unknown symbol: {}", token.value));
    m_scanner->forward();
    return token;
}

}  // namespace PL0