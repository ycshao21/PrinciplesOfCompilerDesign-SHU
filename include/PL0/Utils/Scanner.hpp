#pragma once
#include <fstream>
#include <functional>
#include <string>

namespace PL0
{
/**
 * @brief 扫描器，用于读取 PL/0 源文件。
 */
class Scanner
{
public:
    /**
     * @param filename PL/0 源文件的路径。
     * @throw std::runtime_error 如果无法打开文件。
     * @note 文件指针会指向第一个字符。
     */
    Scanner(const std::string& filename);
    ~Scanner();

public:
    /**
     * @return 当前字符。
     */
    inline char get() const
    {
        return m_curChar;
    }

    /**
     * @return 当前字符的字符串形式。
     */
    inline std::string getAsStr() const
    {
        return std::string(1, m_curChar);
    }

    /**
     * @brief 持续读取字符，直到函数 `fn` 返回 false。
     * @param fn 用于判断是否继续读取的函数。
     * @return 读取到的字符。
     * @note `fn` 不会判断第一个字符。
     */
    std::string getUntil(std::function<bool(char)> fn);

    /**
     * @brief 跳过空格和注释，停在第一个有效字符处。
     */
    void skipSpaceAndComments();
    /**
     * @brief 将指针移向下一个字符。
    */
    void forward();

private:
    std::ifstream m_file;
    char m_curChar = EOF;
};
}  // namespace PL0
