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

/**
 * @brief Translate the token to a symbol.
 * @param token The token.
 * @return The symbol.
*/
Symbol translate2Symbol(const Token& token);

/**
 * @brief Translate the token value to a symbol.
 * @param tokenValue The token value.
 * @return The symbol.
 * @warning This function is not suitable for keywords.
*/
Symbol translate2Symbol(const std::string& tokenValue);

///////////////////////////////////////////////////////////////////////////
// Element (for semantic analysis)
///////////////////////////////////////////////////////////////////////////

enum class SymbolType
{
    TERMINAL = 0,
    NON_TERMINAL,
    ACTION,
    SYNTHESIZED,
    ENDSYM
};

/**
 * @brief The element for semantic analysis.
 * @note For terminal symbols,
 *          - symbol: The terminal symbol. e.g. "+"
 *          - type: SymbolType::TERMINAL.
 *          - values: (empty)
 *       For non-terminal symbols,
 *          - symbol: The non-terminal symbol. e.g. "E"
 *          - type: SymbolType::NON_TERMINAL.
 *          - values: empty, or the value of the non-terminal symbol.
 *       For action symbols,
 *          - symbol: The action symbol. e.g. "10"
 *          - type: SymbolType::ACTION.
 *          - values: The operands for the action function.
 *       For synthesized symbols,
 *          - symbol: The synthesized symbol. e.g. "E"
 *          - type: SymbolType::SYNTHESIZED.
 *          - values: The value of the synthesized symbol.
 *       For the end symbol,
 *          - symbol: ENDSYM
 *          - type: SymbolType::ENDSYM.
 *          - values: (empty)
*/
struct Element
{
    Symbol symbol;
    SymbolType type;
    std::vector<int> values;

    Element(const Symbol& sym, bool synthesized = false);
};

}  // namespace PL0
