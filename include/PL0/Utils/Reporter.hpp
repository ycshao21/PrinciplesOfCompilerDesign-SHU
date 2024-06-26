#pragma once
#include <format>
#include <iostream>
#include <string>

namespace PL0
{
class Reporter
{
public:
    static void info(const std::string& message)
    {
        std::cout << std::format("{}INFO >>> {}\n{}", m_infoColor, message, m_resetColor);
    }

    static void success(const std::string& message)
    {
        std::cout << std::format("{}SUCCESS >>> {}\n{}", m_successColor, message, m_resetColor);
    }

    static void error(const std::string& message)
    {
        std::cout << std::format("{}ERROR >>> {}\n{}", m_errorColor, message, m_resetColor);
    }

private:
    static constexpr const char* m_infoColor = "\033[1;37m";   // White
    static constexpr const char* m_successColor = "\033[1;32m";  // Green
    static constexpr const char* m_warnColor = "\033[1;33m";   // Yellow
    static constexpr const char* m_errorColor = "\033[1;31m";  // Red
    static constexpr const char* m_resetColor = "\033[0m";
};

}  // namespace PL0
