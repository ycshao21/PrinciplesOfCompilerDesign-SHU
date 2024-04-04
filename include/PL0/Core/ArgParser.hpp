#pragma once
#include <optional>
#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <string_view>
#include <unordered_map>

namespace PL0
{
class ArgParser
{
public:
    ArgParser() = default;
    ~ArgParser() = default;

    void addOption(std::string_view name, std::string_view description, std::string_view type,
                   std::optional<std::string> defaultValue = std::nullopt);
    bool parse(int argc, char* argv[]) noexcept;

    template <typename T>
    std::optional<T> get(std::string_view name) const
    {
        auto it = m_options.find(name);
        if (it == m_options.end() || !it->second.value) {
            return std::nullopt;
        }
        std::stringstream ss;
        ss << it->second.value.value();
        T value;
        ss >> value;
        if (ss.fail()) {
            std::cout << std::format("Failed to convert {} to type {}", it->second.value.value(),
                                     it->second.type);
            return std::nullopt;
        }
        return value;
    }

private:
    struct Option
    {
        std::string description;
        std::string type;
        std::optional<std::string> value;
    };
    std::unordered_map<std::string_view, Option> m_options;
};
}  // namespace PL0
