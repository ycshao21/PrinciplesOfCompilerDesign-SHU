#include "PL0/Core/Lexer.hpp"
#include "PL0/Utils/Reporter.hpp"

#include <algorithm>
#include <string>

namespace PL0
{
Word Lexer::getNextWord()
{
    m_scanner.skipSpaceAndComments();
    char ch = m_scanner.getChar();

    if (isAlpha(ch)) {
        return getKeywordOrIdentifier();
    } else if (isDigit(ch)) {
        return getNumber();
    } else if (isDelimiter(ch)) {
        return getDelimiter();
    } else if (isOperatorChar(ch)) {
        return getOperator();
    } else if (ch == EOF) {
        return {WordType::EndOfFile, ""};
    } else {
        return getUnknownSymbol();
    }
}

Word Lexer::getNumber()
{
    Word word{WordType::Number, m_scanner.getUntil(isDigit)};
    if (isAlpha(m_scanner.getChar())) {
        word.type = WordType::Invalid;
        word.value += m_scanner.getUntil(isAlphaOrDigit);
        // Reporter::error("Invalid identifier: {}", word.value);
        Reporter::error(std::format("Invalid identifier: {}", word.value));
    }
    return word;
}

Word Lexer::getDelimiter()
{
    Word word = {WordType::Delimiter, m_scanner.getAsString()};
    m_scanner.forward();
    return word;
}

Word Lexer::getOperator()
{
    Word word{WordType::Operator, m_scanner.getAsString()};
    m_scanner.forward();
    if (word.value == "<" || word.value == ">") {
        if (m_scanner.getChar() == '=') {  // >=, <=
            word.value += '=';
            m_scanner.forward();
        }
    } else if (word.value == ":") {
        if (m_scanner.getChar() == '=') {  // :=
            word.value += '=';
            m_scanner.forward();
        } else {
            word.type = WordType::Invalid;
            // Reporter::error("Invalid operator", word.value);
            Reporter::error(std::format("Invalid operator: {}", word.value));
        }
    }
    return word;
}

Word Lexer::getKeywordOrIdentifier()
{
    Word word;
    word.value = m_scanner.getUntil(isAlphaOrDigit);
    std::ranges::transform(word.value, word.value.begin(), ::tolower);
    if (std::ranges::find(KEYWORDS, word.value) != KEYWORDS.end()) {
        word.type = WordType::Keyword;
    } else {
        word.type = WordType::Identifier;
    }
    return word;
}

Word Lexer::getUnknownSymbol()
{
    Word word{WordType::Invalid, m_scanner.getAsString()};
    // Reporter::error("Unknown symbol", word.value);
    Reporter::error(std::format("Unknown symbol: {}", word.value));
    m_scanner.forward();
    return word;
}

}  // namespace PL0