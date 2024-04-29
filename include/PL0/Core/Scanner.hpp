#pragma once
#include <fstream>
#include <functional>
#include <string>

namespace PL0
{
class Scanner
{
public:
    Scanner(const std::string& filename);
    ~Scanner();

    inline char get() const
    {
        return m_curChar;
    }

    inline std::string getAsStr() const
    {
        return std::string(1, m_curChar);
    }

    // Get the chars until the function {fn} returns false.
    // [NOTE] {fn} does not apply to the first char.
    std::string getUntil(std::function<bool(char)> fn);

    void skipSpaceAndComments();
    void forward();

private:
    std::ifstream m_file;
    char m_curChar = EOF;
};
}  // namespace PL0
