#pragma once
#include <fstream>
#include <string>
#include <functional>

namespace PL0
{
class Scanner
{
public:
    Scanner(const std::string& filename);
    ~Scanner();

    int getLineNumber() const
    {
        return m_line;
    }
    char getChar() const
    {
        return m_curChar;
    }
    std::string getAsString() const
    {
        return std::string(1, m_curChar);
    }
    // Get the chars until the function {fn} returns false.
    // [Note] {fn} does not apply to the first char.
    std::string getUntil(std::function<bool(char)> fn);

    void skipSpaceAndComments();
    void forward();

private:
    std::ifstream m_file;
    char m_curChar = EOF;
    int m_line = 1;
};
}  // namespace PL0
