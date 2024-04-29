#include "PL0/Utils/ArgParser.hpp"
#include <iostream>

namespace PL0
{
void ArgParser::addOption(std::string_view name, std::string_view description,
                          std::string_view type, std::optional<std::string> defaultValue)
{
    if (m_options.find(name) != m_options.end()) {
        std::cout << "Option \"" << name << "\" already exists" << std::endl;
        return;
    }
    if (type != "string" && type != "int" && type != "float" && type != "bool") {
        std::cout << "Invalid type: \"" << type
                  << "\"; Supported types are: string, int, float, bool" << std::endl;
        return;
    }
    if (defaultValue && type != "string") {
        if (type == "int") {
            try {
                std::stoi(defaultValue.value());
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid default value for option \"" << name
                          << "\"; Expected type: int" << std::endl;
                return;
            }
        } else if (type == "float") {
            try {
                std::stof(defaultValue.value());
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid default value for option \"" << name
                          << "\"; Expected type: float" << std::endl;
                return;
            }
        } else if (type == "bool") {
            if (defaultValue.value() != "true" && defaultValue.value() != "false") {
                std::cout << "Invalid default value for option \"" << name
                          << "\"; Expected type: bool" << std::endl;
                return;
            }
        }
    }
    m_options[name] = {std::string(description), std::string(type), defaultValue};
}

bool ArgParser::parse(int argc, char* argv[]) noexcept
{
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        size_t dashCount = 0;
        if (arg.starts_with("--")) {
            dashCount = 2;
        } else if (arg.starts_with("-")) {
            dashCount = 1;
        } else {
            std::cout << std::format("Invalid argument: \"{}\"; "
                                      "An argument must starts with \"-\" or \"--\"",
                                      arg) << std::endl;
            return false;
        }
        arg.remove_prefix(dashCount);
        auto it = m_options.find(arg);
        if (it != m_options.end()) {
            if (i + 1 < argc) {
                it->second.value = argv[i + 1];
                ++i;
            } else {
                std::cout << std::format("Option \"{}\" requires a value", arg) << std::endl;
                return false;
            }
        } else {
            std::cout << std::format("Unknown option: \"{}\"", arg) << std::endl;
            return false;
        }
    }
    return true;
}
}  // namespace PL0