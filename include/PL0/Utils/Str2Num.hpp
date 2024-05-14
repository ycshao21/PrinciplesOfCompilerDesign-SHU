#pragma once
#include <string>
#include <optional>
#include <stdexcept>

namespace PL0
{
/**
 * @brief 将字符串转换为整数。
 * @param str 可能表示数字的字符串。
 * @return 如果字符串是数字，则返回数字；否则返回 std::nullopt。
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

