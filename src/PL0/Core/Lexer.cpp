#include "PL0/Core/Lexer.hpp"
#include "PL0/Utils/Reporter.hpp"

#include <algorithm>
#include <string>

namespace PL0
{
std::vector<Token> Lexer::tokenize(const std::string& srcFile)
{
    // 初始化扫描器
    m_scanner = std::make_unique<Scanner>(srcFile);

    std::vector<Token> tokens;
    while (true) {
        m_scanner->skipSpaceAndComments();
        char c = m_scanner->get();

        if (c == EOF) {  // 所有字符都已经读取完毕
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
     * @note 关键字和标识符的第一个字符一定是字母。
     *      对于关键字，后续字符都是字母；
     *      对于标识符，后续字符可以是字母或数字。
     */
    Token token;
    token.value = m_scanner->getUntil(isAlphaOrDigit);
    std::ranges::transform(token.value, token.value.begin(), ::tolower);  // Convert to lowercase

    /**
     * @note 如果该单词在`KEYWORDS`数组中，那么它是一个关键字；否则，将其视为标识符（在词法分析阶段）。
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
     * @note 数字类型的单词只能由数字组成。
     */

    Token token{TokenType::Number, m_scanner->getUntil(isDigit)};

    /**
     * @note 如果下一个字符是字母，我们将其视为以数字开头的无效标识符，例如：1abc。
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
     * @note 界符只有单个字符。
     */

    Token token = {TokenType::Delimiter, m_scanner->getAsStr()};
    m_scanner->forward();
    return token;
}

Token Lexer::getOperator()
{
    /**
     * @note 操作符可以是单个字符或两个字符的字符串。
     *      两个字符的操作符有：<=, >=, :=。
     *      对于 <, >，它们后面可以跟 '=' 或不跟。
     *      对于 :, 它后面必须跟 '='，否则是一个无效的操作符。
     */

    Token token{TokenType::Operator, m_scanner->getAsStr()};

    // 检查操作符是否是两个字符的字符串
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