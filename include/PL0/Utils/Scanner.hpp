#pragma once
#include <fstream>
#include <functional>
#include <string>

namespace PL0
{
/**
 * @brief A character scanner for reading the PL/0 source file.
 */
class Scanner
{
public:
    /**
     * @param filename The path to the PL/0 source file.
     * @throw std::runtime_error If the file cannot be opened.
     * @note The cursor will be set to the first character.
     */
    Scanner(const std::string& filename);
    ~Scanner();

public:
    /**
     * @return The current character.
     */
    inline char get() const
    {
        return m_curChar;
    }

    /**
     * @return The current character as a string.
     */
    inline std::string getAsStr() const
    {
        return std::string(1, m_curChar);
    }

    /**
     * @brief Get the characters until the function {fn} returns false.
     * @param fn The function to determine whether to continue reading.
     * @return The characters read.
     * @note {fn} will not determine the first character.
     */
    std::string getUntil(std::function<bool(char)> fn);

    /**
     * @brief Skip the spaces and comments, and stop at the first available character.
     */
    void skipSpaceAndComments();
    /**
     * @brief Move the cursor to the next character.
     */
    void forward();

private:
    std::ifstream m_file;
    char m_curChar = EOF;
};
}  // namespace PL0
