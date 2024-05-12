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
     * @note The syntax of arithmetic expressions:
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
     * @note Input stack (The bottom is at index 0):
     *   a + 5 * b  =>
     *   ----------------------------
     *   | ENDSYM  id  *  num  +  id   <---
     *   ----------------------------
     */
    std::vector<Symbol> inputStack{ENDSYM};

    // Tranlate all tokens to symbols and push them in reverse order.
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        Symbol symbol = translate2Symbol(*it);
        inputStack.push_back(symbol);
    }

    /**
     * @note Analysis stack (The bottom is at index 0):
     *   Initial state:
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
             * @note If the top of the analysis stack is a terminal symbol or the end symbol,
             *      then it should match the top of the input stack.
             */
            if (m_analyzer.isTerminal(atop) || atop == ENDSYM) {
                if (atop != itop) {  // Mismatch
                    throw SyntaxError(std::format(
                        "The terminal symbol {} does not match the top of the input stack {}.",
                        atop, itop));
                }

                // Pop analysis stack and input stack.
                analysisStack.pop_back();
                inputStack.pop_back();
            } else {
                /**
                 * @note If the top of the analysis stack is a non-terminal symbol X,
                 *     find the production rule X -> Y1Y2...Yn in the prediction table,
                 *     and replace X with Y1Y2...Yn (in reverse order) in the analysis stack (except ε).
                 */

                const auto& items = m_predictionTable[atop];
                if (items.find(itop) == items.end()) {  // No such production rule
                    throw SyntaxError(std::format("{} is not allowed.", itop));
                }
                const auto& rhs = m_predictionTable[atop][itop];

                // 1) Pop X
                analysisStack.pop_back();

                // 2) Push Yn, Yn-1, ..., Y1
                for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
                    if (*it != EPSILON) {  // Skip ε
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
     * @note It is impossible that one of the stacks is empty while the other is not.
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