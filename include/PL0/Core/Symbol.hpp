#pragma once
#include <string>
#include <functional>
#include <optional>
#include <vector>

#include "Token.hpp"
#include "Action.hpp"

namespace PL0
{
using Symbol = std::string;

constexpr Symbol EPSILON = "";
constexpr Symbol ENDSYM = "##";

enum class SymbolType
{
    TERMINAL = 0,
    NON_TERMINAL,
    ACTION,
    SYNTHESIZED,
    ENDSYM
};

struct Element
{
    Symbol symbol;
    SymbolType type;
    std::vector<int> values;

    Element(const Symbol& sym, bool synthesized = false)
        : symbol(sym)
    {
        if (sym == ENDSYM) {
            type = SymbolType::ENDSYM;
        } else if (std::isdigit(sym[0])) {
            type = SymbolType::ACTION;
        } else if (std::isupper(sym[0])) {
            if (synthesized) {
                type = SymbolType::SYNTHESIZED;
            } else {
                type = SymbolType::NON_TERMINAL;
            }
        } else {
            type = SymbolType::TERMINAL;
        }
    }
};


std::string translate2Symbol(const Token& token);

// [WARNING] This function is not suitable for keywords.
std::string translate2Symbol(const std::string& tokenValue);
}  // namespace PL0
