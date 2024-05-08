#include "PL0/Core/LL1Parser.hpp"
#include "PL0/Utils/Reporter.hpp"
#include <format>
#include <stack>

namespace PL0
{
LL1Parser::LL1Parser()
{
    m_syntax.setBeginSym("E");
    // Arithmetic expression:
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

    m_syntax.addRule("E", {"+", "T", "E'"});
    m_syntax.addRule("E", {"-", "T", "E'"});
    m_syntax.addRule("E", {"T", "E'"});
    m_syntax.addRule("E'", {"+", "T", "E'"});
    m_syntax.addRule("E'", {"-", "T", "E'"});
    m_syntax.addRule("E'", {EPSILON});
    m_syntax.addRule("T", {"F", "T'"});
    m_syntax.addRule("T'", {"*", "F", "T'"});
    m_syntax.addRule("T'", {"/", "F", "T'"});
    m_syntax.addRule("T'", {EPSILON});
    m_syntax.addRule("F", {"(", "E", ")"});
    m_syntax.addRule("F", {"id"});
    m_syntax.addRule("F", {"num"});

    m_syntax.calcSelectSets();
    generatePredictionTable();

    // m_syntax.printResults();
}

void LL1Parser::generatePredictionTable()
{
    const auto& rules = m_syntax.getRules();
    for (size_t i = 0; i < rules.size(); ++i) {
        const auto& rule = rules[i];
        const Symbol& lhs = rule.lhs;
        const std::set<Symbol>& selectSet = m_syntax.getSelectSet(i);

        for (const Symbol& sym : selectSet) {
            m_table[lhs][sym] = rule.rhs;
        }
    }

    // printTable();
}

void LL1Parser::printTable()
{
    // E -- ( -> TE', id -> TE', num -> TE',
    // E' -- # -> ε, ) -> ε, + -> +TE', - -> -TE',
    // F -- ( -> (E), id -> id, num -> num,
    // T -- ( -> FT', id -> FT', num -> FT',
    // T' -- # -> ε, ) -> ε, * -> *FT', + -> ε, - -> ε, / -> /FT',

    for (const auto& [lhs, item] : m_table) {
        std::cout << lhs << " -- ";
        for (const auto& [sym, rule] : item) {
            std::cout << sym << " -> ";
            for (const auto& s : rule) {
                if (!s.empty()) {
                    std::cout << s;
                } else {
                    std::cout << "ε";
                }
            }
            std::cout << ", ";
        }
        std::cout << "\n";
    }
}

void LL1Parser::parse(const std::vector<Token>& tokens)
{
    std::vector<Symbol> restInput;
    restInput.push_back("#");
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        Symbol symbol = translate2Symbol(*it);
        restInput.push_back(symbol);
    }

    std::vector<Symbol> analysisStack;
    analysisStack.push_back("#");
    analysisStack.push_back(m_syntax.getBeginSym());

    while (!analysisStack.empty() && !restInput.empty()) {
        Symbol atop = analysisStack.back();
        Symbol rtop = restInput.back();

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

        if (m_syntax.isTerminal(atop) || atop == "#")
        {
            if (atop != rtop) {
                Reporter::error(std::format("Syntax error: The terminal symbol {} does not match "
                                            "the top of the input stack {}.",
                                            atop, rtop));
                return;
            }
            // Match terminal symbol
            analysisStack.pop_back();
            restInput.pop_back();
        } else {
            const auto& allRules = m_table[atop];
            auto itemIt = allRules.find(rtop);
            // Production rules not found
            if (itemIt == allRules.end()) {
                // std::string symStr;
                // for (const auto& [sym, _] : allRules) {
                //     symStr += sym + ", ";
                // }
                // Reporter::error(std::format("Syntax error: expected {}but got {}.", symStr, rtop));
                Reporter::error(std::format("Syntax error: {} is not allowed.", rtop));

                return;
            }
            const auto& rule = m_table[atop][rtop];

            // Replace the top of the analysis stack with the right-hand side of the rule.
            analysisStack.pop_back();
            for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                if (!it->empty()) {
                    analysisStack.push_back(*it);
                }
            }
        }
    }

    Reporter::info("Syntax correct.");
}

}  // namespace PL0