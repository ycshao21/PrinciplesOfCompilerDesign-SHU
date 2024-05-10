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
    // Rest input
    // e.g. a + 5 * b  =>  a + 5 * b #
    //                      ------->
    std::vector<Symbol> restInput{ENDSYM};
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        Symbol symbol = translate2Symbol(*it);
        restInput.push_back(symbol);
    }

    // Analysis stack
    // Initial state (The bottom is at index 0):
    // ----------------
    // |  #  S      <---
    // ----------------
    std::vector<Symbol> analysisStack{ENDSYM, m_analyzer.getBeginSym()};

    try {
        while (!analysisStack.empty() && !restInput.empty()) {
            Symbol atop = analysisStack.back();
            Symbol rtop = restInput.back();

            if (m_analyzer.isTerminal(atop) || atop == ENDSYM) {
                if (atop != rtop) {
                    throw SyntaxError(
                        std::format("Syntax error: The terminal symbol {} does not match "
                                    "the top of the input stack {}.",
                                    atop, rtop));
                }

                ///////////////////////
                //    Top matched.   //
                ///////////////////////

                // Pop analysis stack and rest input.
                analysisStack.pop_back();
                restInput.pop_back();
            } else {
                //////////////////////////////////////////////////////////////////////
                // Replace the top non-terminal symbol X with the corresponding rule.
                //////////////////////////////////////////////////////////////////////

                // If there is no such rule, throw a syntax error.
                const auto& allRules = m_predictionTable[atop];
                if (allRules.find(rtop) == allRules.end()) {
                    throw SyntaxError(std::format(
                        "Syntax error: No production rules found for {} -> {}.", atop, rtop));
                }
                const auto& rule = m_predictionTable[atop][rtop];

                // 1) Pop X
                analysisStack.pop_back();

                // 2) Push the symbols of the rule in reverse order.
                for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                    if (!it->empty()) {
                        analysisStack.push_back(*it);
                    }
                }
            }
        }
    } catch (const SyntaxError& e) {
        Reporter::error(e.what());
        return;
    }

    Reporter::info("Syntax correct.");
}

void LL1Parser::initSyntax()
{
    // Arithmetic expression:
    //   S -> E
    //   E -> + T E'
    //   E -> - T E'
    //   E -> T E'
    //   E' -> + T E'
    //   E' -> - T E'
    //   E' -> ε
    //   T -> F T'
    //   T' -> * F T'
    //   T' -> / F T'
    //   T' -> ε
    //   F -> ( E )
    //   F -> id
    //   F -> num
    //
    // where E is for expression, T is for term, F is for factor.

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

    // m_analyzer.printResults();
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
                           const std::vector<Symbol>& restInput)
{
    std::cout << "Analysis stack: ";
    for (const auto& sym : analysisStack) {
        std::cout << sym << " ";
    }
    std::cout << "\n";

    std::cout << "Rest input: ";
    for (auto it = restInput.rbegin(); it != restInput.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    std::cout << "--------------------------------------------------------------------------\n";
}

}  // namespace PL0