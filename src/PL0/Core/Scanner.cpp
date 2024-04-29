#include "PL0/Core/Scanner.hpp"

namespace PL0
{
Scanner::Scanner(const std::string& filename) : m_file(filename), m_curChar(EOF)
{
    if (!m_file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    m_curChar = m_file.get();
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
        while (std::isspace(m_curChar)) {
            forward();
        }
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