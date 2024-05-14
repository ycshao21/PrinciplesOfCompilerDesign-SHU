#include "PL0/Core/LL1Parser.hpp"
#include "PL0/Utils/Exception.hpp"
#include "PL0/Utils/Reporter.hpp"
#include <format>
#include <stack>

namespace PL0
{
LL1Parser::LL1Parser()
{
    initSyntax();
}

void LL1Parser::initSyntax()
{
    /**
     * @note 算术表达式的文法：
     *  S -> E
     *  E -> + T E'
     *  E -> - T E'
     *  E -> T E'
     *  E' -> + T E'
     *  E' -> - T E'
     *  E' -> ε
     *  T -> F T'
     *  T' -> * F T'
     *  T' -> / F T'
     *  T' -> ε
     *  F -> ( E )
     *  F -> id
     *  F -> num
     */

    m_analyzer.setBeginSym("S");

    m_analyzer.addRule("S", {"E"});
    m_analyzer.addRule("E", {"+", "E'"});
    m_analyzer.addRule("E", {"-", "E'"});
    m_analyzer.addRule("E", {"E'"});
    m_analyzer.addRule("E'", {"T", "E''"});
    m_analyzer.addRule("E''", {"+", "T", "E''"});
    m_analyzer.addRule("E''", {"-", "T", "E''"});
    m_analyzer.addRule("E''", {EPSILON});
    m_analyzer.addRule("T", {"F", "T'"});
    m_analyzer.addRule("T'", {"*", "F", "T'"});
    m_analyzer.addRule("T'", {"/", "F", "T'"});
    m_analyzer.addRule("T'", {EPSILON});
    m_analyzer.addRule("F", {"(", "E", ")"});
    m_analyzer.addRule("F", {"id"});
    m_analyzer.addRule("F", {"num"});

    m_analyzer.calcSelectSets();
    generatePredictionTable();
}

void LL1Parser::generatePredictionTable()
{
    const auto& rules = m_analyzer.getRules();
    for (size_t i = 0; i < rules.size(); ++i) {
        const auto& rule = rules[i];
        const Symbol& lhs = rule.lhs;
        const std::set<Symbol>& selectSet = m_analyzer.getSelectSet(i);

        for (const Symbol& sym : selectSet) {
            m_predictionTable[lhs][sym] = rule.rhs;
        }
    }
}

void LL1Parser::parse(const std::vector<Token>& tokens)
{
    /**
     * @note 剩余输入串（栈底下标为 0）：
     *   a + 5 * b  =>
     *   ----------------------------
     *   | ENDSYM  id  *  num  +  id   <---
     *   ----------------------------
     */
    std::vector<Symbol> inputStack{ENDSYM};

    // 将所有词转换为符号，并逆序推入栈中。
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        Symbol symbol = translate2Symbol(*it);
        inputStack.push_back(symbol);
    }

    /**
     * @note 分析栈（栈底下标为 0）：
     *   初始状态：
     *    --------------------
     *    |  ENDSYM  BEGINSYM      <---
     *    --------------------
     */
    std::vector<Symbol> analysisStack{ENDSYM, m_analyzer.getBeginSym()};

    try {
        while (!analysisStack.empty() && !inputStack.empty()) {
            Symbol atop = analysisStack.back();
            Symbol itop = inputStack.back();

            /**
             * @note 如果分析栈顶部是终结符或结束符号，则正常情况下应该与输入栈顶部匹配。
            */
            if (m_analyzer.isTerminal(atop) || atop == ENDSYM) {
                if (atop != itop) {  // Mismatch
                    throw SyntaxError(std::format(
                        "The terminal symbol {} does not match the top of the input stack {}.",
                        atop, itop));
                }

                // 分析栈顶部和输入栈顶部匹配，因此将它们弹出。
                analysisStack.pop_back();
                inputStack.pop_back();
            } else {
                /**
                 * @note 如果分析栈顶部是非终结符 X，
                 *      在预测分析表中查找产生式 X -> Y1Y2...Yn，
                 *      并将 X 出栈，将 Y1Y2...Yn 中的非空符号逆序推入分析栈。
                */

                const auto& items = m_predictionTable[atop];
                if (items.find(itop) == items.end()) {  // 规则不存在
                    throw SyntaxError(std::format("{} is not allowed.", itop));
                }
                const auto& rhs = m_predictionTable[atop][itop];

                analysisStack.pop_back();

                for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
                    if (*it != EPSILON) {  // 跳过 ε
                        analysisStack.push_back(*it);
                    }
                }
            }
        }
    } catch (const SyntaxError& e) {
        Reporter::error(e.what());
        return;
    }

    /**
     * @note 如果分析栈和输入栈都为空，则语法正确。
     *      不可能存在其中一个栈为空而另一个不为空的情况。
    */
    Reporter::success("Syntax correct.");
}

void LL1Parser::printPredictionTable()
{
    for (const auto& [lhs, item] : m_predictionTable) {
        std::cout << lhs << " -- ";
        for (const auto& [sym, rule] : item) {
            std::cout << sym << " -> ";
            for (const auto& s : rule) {
                if (s == EPSILON) {
                    std::cout << "ε";
                } else {
                    std::cout << s;
                }
            }
            std::cout << ", ";
        }
        std::cout << "\n";
    }
}

void LL1Parser::printState(const std::vector<Symbol>& analysisStack,
                           const std::vector<Symbol>& inputStack)
{
    std::cout << "Analysis stack: ";
    for (const auto& sym : analysisStack) {
        std::cout << sym << " ";
    }
    std::cout << "\n";

    std::cout << "Input stack: ";
    for (auto it = inputStack.rbegin(); it != inputStack.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    std::cout << "--------------------------------------------------------------------------\n";
}
}  // namespace PL0