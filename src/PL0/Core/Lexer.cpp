#include "PL0/Core/Lexer.hpp"
#include "PL0/Utils/Reporter.hpp"

#include <algorithm>
#include <string>

namespace PL0
{
std::vector<Token> Lexer::tokenize(const std::string& srcFile)
{
    // Initialize the scanner
    m_scanner = std::make_unique<Scanner>(srcFile);

    std::vector<Token> tokens;
    while (true) {
        m_scanner->skipSpaceAndComments();
        char c = m_scanner->get();  // First available character of a token

        if (c == EOF) {  // All tokens have been scanned.
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

Token Lexer::getKeywordOrIdentifier()
{
    /**
     * @note The first character of a keyword or an identifier must be an alphabet.
     *      For keywords, the following characters are all alphabets.
     *      For identifiers, the following characters can be either alphabets or digits.
     */
    Token token;
    token.value = m_scanner->getUntil(isAlphaOrDigit);
    std::ranges::transform(token.value, token.value.begin(), ::tolower);  // Convert to lowercase

    /**
     * @note If the token is found in the KEYWORDS array, it is a keyword.
     *      Otherwise, we treat it as an identifier (in the lexical phase).
     */
    if (std::ranges::find(KEYWORDS, token.value) != KEYWORDS.end()) {
        token.type = TokenType::Keyword;
    } else {
        token.type = TokenType::Identifier;
    }
    return token;
}

Token Lexer::getNumber()
{
    /**
     * @note All the characters of a number must be digits.
     */

    Token token{TokenType::Number, m_scanner->getUntil(isDigit)};

    /**
     * @note If the next character is an alphabet,
     *      we treat it as an invalid identifier which begins with a digit.
     */
    if (isAlpha(m_scanner->get())) {
        token.type = TokenType::Invalid;
        token.value += m_scanner->getUntil(isAlphaOrDigit);
        Reporter::error(std::format("Invalid identifier: {}", token.value));
    }
    return token;
}

Token Lexer::getDelimiter()
{
    /**
     * @note The delimiter is a single character.
     */

    Token token = {TokenType::Delimiter, m_scanner->getAsStr()};
    m_scanner->forward();
    return token;
}

Token Lexer::getOperator()
{
    /**
     * @note The operator can be a single character or a two-character sequence.
     *     The two-character operators are: <=, >=, :=.
     *     For <, >, they can be followed by '=' or not.
     *     For :, it must be followed by '='. Otherwise, it is an invalid operator.
     */

    Token token{TokenType::Operator, m_scanner->getAsStr()};

    // Check if the operator is a two-character sequence
    m_scanner->forward();
    char c = m_scanner->get();
    if (token.value == "<" || token.value == ">") {
        if (c == '=') {  // >=, <=
            token.value += '=';
            m_scanner->forward();
        }
    } else if (token.value == ":") {
        if (c == '=') {  // :=
            token.value += '=';
            m_scanner->forward();
        } else {
            token.type = TokenType::Invalid;
            Reporter::error(std::format("Invalid operator: {}", token.value));
        }
    }
    return token;
}

Token Lexer::getUnknownSymbol()
{
    Token token{TokenType::Invalid, m_scanner->getAsStr()};
    Reporter::error(std::format("Unknown symbol: {}", token.value));
    m_scanner->forward();
    return token;
}

std::string Lexer::encode(const Token& token) noexcept
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
        }
    }
    }
    return "nul";
}

}  // namespace PL0