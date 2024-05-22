#pragma once
#include "PL0/Utils/Scanner.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>

namespace PL0
{
/**
 * @brief 词法分析器
*/
class Lexer
{
public:
    Lexer() = default;
    ~Lexer() = default;

    /**
     * @brief 将 PL/0 源代码转换为单词序列。
     * @param srcFile PL/0 源文件的路径。
     * @note 词法分析器会检查出所有无效的单词。
    */
    std::vector<Token> tokenize(const std::string& srcFile);

    /**
     * @brief 将单词编码为字符串（供实验二实验）
     * @param token 待编码的单词。
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
