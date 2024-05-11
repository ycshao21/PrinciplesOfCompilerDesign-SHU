#include "PL0/Utils/Scanner.hpp"

namespace PL0
{
Scanner::Scanner(const std::string& filename) : m_file(filename)
{
    if (!m_file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Set the cursor to the first character of the PL/0 source file.
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
        // Skip the spaces
        while (std::isspace(m_curChar)) {
            forward();
        }

        // Skip the comments
        /**
         * @note The comments are enclosed by '{' and '}'.
         *       e.g. {This is a comment}
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