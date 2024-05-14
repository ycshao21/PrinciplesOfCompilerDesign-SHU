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
 * @brief 将词转换为符号。
 * @param token 带转换的词。
 * @return 转换后的符号。
*/
Symbol translate2Symbol(const Token& token);

/**
 * @brief 将词值转换为符号。
 * @param tokenValue 带转换的词值。
 * @return 转换后的符号。
 * @warning 该函数不适用于关键字。
*/
Symbol translate2Symbol(const std::string& tokenValue);

///////////////////////////////////////////////////////////////////////////
// 元素（用于语义分析）
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
 * @brief 语义分析元素。
 * @note 对于终结符，
 *          - symbol: 终结符，e.g. "+"
 *          - type: SymbolType::TERMINAL.
 *          - values: (空)
 *       对于非终结符，
 *          - symbol: 非终结符，e.g. "E"
 *          - type: SymbolType::NON_TERMINAL.
 *          - values: 空，或者是非终结符携带的值。
 *       对于动作符号，
 *          - symbol: 动作符号，e.g. "10"
 *          - type: SymbolType::ACTION.
 *          - values: 动作函数的操作数。
 *       对于综合属性代表的符号，
 *          - symbol: 综合属性代表的符号，e.g. "T"
 *          - type: SymbolType::SYNTHESIZED.
 *          - values: 综合属性的值。
 *       对于结束符号，
 *          - symbol: ENDSYM
 *          - type: SymbolType::ENDSYM.
 *          - values: (空)
*/
struct Element
{
    Symbol symbol;
    SymbolType type;
    std::vector<int> values;

    Element(const Symbol& sym, bool synthesized = false);
};

}  // namespace PL0
