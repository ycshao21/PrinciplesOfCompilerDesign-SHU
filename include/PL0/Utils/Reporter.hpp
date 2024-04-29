#pragma once
#include <format>
#include <iostream>
#include <string>

namespace PL0
{
class Reporter
{
public:

    static void error(const std::string& message)
    {
        std::cout << std::format("{}ERROR >>> {}\n{}", m_errorColor, message, m_resetColor);
    }

private:
    // static constexpr const char* m_infoColor = "\033[1;37m";   // White
    // static constexpr const char* m_warnColor = "\033[1;33m";   // Yellow
    static constexpr const char* m_errorColor = "\033[1;31m";  // Red
    static constexpr const char* m_resetColor = "\033[0m";
};

}  // namespace PL0
