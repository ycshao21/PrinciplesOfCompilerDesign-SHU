#pragma once
#include <string>
#include <optional>
#include <stdexcept>

namespace PL0
{
/**
 * @brief Convert a string to an integer.
 * @param str The string that may represent a number.
 * @return The number if the string is a number; otherwise, std::nullopt.
 */
std::optional<int> str2num(const std::string& str)
{
    try {
        int num = std::stoi(str);
        return std::make_optional(num);
    } catch (const std::invalid_argument& e) {
        return std::nullopt;
    }
}
    
} // namespace PL0

