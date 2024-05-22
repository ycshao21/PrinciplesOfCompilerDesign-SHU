#include "PL0/Utils/Scanner.hpp"

namespace PL0
{
Scanner::Scanner(const std::string& filename) : m_file(filename)
{
    if (!m_file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // 将指针指向 PL/0 源文件的第一个字符。
    forward();
}

Scanner::~Scanner()
{
    m_file.close();
}

void Scanner::forward()
{
    m_curChar = m_file.get();
}

void Scanner::skipSpaceAndComments()
{
    while (true) {
        // 跳过空白字符
        while (std::isspace(m_curChar)) {
            forward();
        }

        // 跳过注释
        /**
         * @note 注释的内容是由 '{' 和 '}' 包围的。
         *       e.g. {This is a comment}
         * 
        */
        if (m_curChar == '{') {
            do {
                forward();
            } while (m_curChar != '}' && m_curChar != EOF);
            if (m_curChar == '}') {
                forward();
            }
        } else {
            return;
        }
    }
}

std::string Scanner::getUntil(std::function<bool(char)> fn){
    std::string str;

    do {
        str += m_curChar;
        forward();
    } while (fn(m_curChar));

    return str;
}
}  // namespace PL0