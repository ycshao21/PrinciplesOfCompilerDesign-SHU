#include "PL0/Core/Syntax.hpp"
#include <format>
#include <iostream>
#include <ranges>

namespace PL0
{
void Syntax::addProductionRule(const Symbol& lhs, const std::vector<Symbol>& rhs)
{
    // Add the production rule
    ProductionRule rule{lhs, rhs};
    m_productionRules.push_back(rule);

    // Mark the left-hand side symbol as non-terminal
    m_nonTerminals.insert(lhs);

    for (const Symbol& sym : rhs) {
        if (sym.empty()) {
            continue;
        }

        if (std::isupper(sym[0])) {
            // Mark the symbol as non-terminal
            m_nonTerminals.insert(sym);
        } else {
            // Mark the symbol as terminal
            m_terminals.insert(sym);
        }
    }
}

void Syntax::calcFirstSet()
{
    m_firstSet.clear();

    // 1. If X is a terminal symbol, then FIRST(X) = {X}.
    for (const Symbol& sym : m_terminals) {
        m_firstSet[sym] = {sym};
    }
    for (const Symbol& sym : m_nonTerminals) {
        m_firstSet[sym] = {};
    }

    bool updated;
    do {
        updated = false;
        for (const auto& rule : m_productionRules) {
            const auto& lhs = rule.lhs;
            const auto& rhs = rule.rhs;

            const auto& firstSym = rhs[0];
            auto& firstSet = m_firstSet[lhs];

            // 2. If X -> a... exists, where a is a terminal, then add a to FIRST(X).
            // 3. If X -> ε, then add ε to FIRST(X).
            if (firstSym.empty() || isTerminal(firstSym)) {
                auto [_, inserted] = firstSet.insert(firstSym);
                if (inserted) {
                    updated = true;
                }
            } else {
                // X -> Y1 Y2 ... Yk, where Y1, Y2, ..., Yk are non-terminals.

                // 4. If Y1, Y2, ..., Yi-1 => ε, then add First(Y1)-{ε}, First(Y2)-{ε}, ...,
                // First(Yi) to FIRST(X).
                bool allNull = true;
                for (const Symbol& sym : rhs) {
                    auto& symFirstSet = m_firstSet[sym];
                    bool hasNull = false;
                    for (const Symbol& s : symFirstSet) {
                        if (s.empty()) {
                            hasNull = true;
                            continue;
                        }

                        // Add non-empty symbols
                        auto [_, inserted] = firstSet.insert(s);
                        if (inserted) {
                            updated = true;
                        }
                    }

                    if (hasNull == false) {
                        allNull = false;
                        break;
                    }
                }

                // 5. If Yi => ε for all i = 1, 2, ..., k, then add ε to FIRST(X).
                if (allNull) {
                    auto [_, inserted] = firstSet.insert("");
                    if (inserted) {
                        updated = true;
                    }
                }
            }
        }
    } while (updated);
}

void Syntax::calcFollowSet()
{
    m_followSet.clear();

    // Initialize the FOLLOW set for each non-terminal symbol
    for (const auto& sym : m_nonTerminals) {
        m_followSet[sym] = {};
    }

    // 1. Add # to the FOLLOW set of the start symbol
    m_followSet[m_beginSym].insert("#");

    bool updated;
    do {
        updated = false;
        for (const auto& rule : m_productionRules) {
            const auto& lhs = rule.lhs;
            const auto& rhs = rule.rhs;

            for (size_t i = 0; i < rhs.size(); ++i) {
                const Symbol& sym = rhs[i];

                // If the symbol is a non-terminal, add the FIRST set of the next symbol to the
                // FOLLOW set of the current symbol.
                if (isNonTerminal(sym)) {
                    bool hasNull = false;
                    size_t j = i + 1;
                    for (; j < rhs.size(); ++j) {
                        // NOTE: It is guaranteed that nextSym is not ε.
                        const Symbol& nextSym = rhs[j];
                        if (isTerminal(nextSym)) {
                            auto [_, inserted] = m_followSet[sym].insert(nextSym);
                            if (inserted) {
                                updated = true;
                            }
                            break;
                        } else {
                            for (const Symbol& s : m_firstSet[nextSym]) {
                                if (!s.empty()) {
                                    auto [_, inserted] = m_followSet[sym].insert(s);
                                    if (inserted) {
                                        updated = true;
                                    }
                                } else {
                                    hasNull = true;
                                }
                            }
                        }

                        if (!hasNull) {
                            break;
                        }
                    }

                    // If the FIRST set of the next symbol contains ε, add the FOLLOW set of the
                    // left-hand side symbol.
                    if (hasNull || j == rhs.size()) {
                        for (const Symbol& s : m_followSet[lhs]) {
                            auto [_, inserted] = m_followSet[sym].insert(s);
                            if (inserted) {
                                updated = true;
                            }
                        }
                    }
                }
            }
        }
    } while (updated);
}

void Syntax::calcSelectSet()
{
    calcFirstSet();
    calcFollowSet();

    m_selectSet.clear();
    m_selectSet.resize(m_productionRules.size());

    for (size_t i = 0; i < m_productionRules.size(); ++i) {
        const ProductionRule& rule = m_productionRules[i];
        const auto& lhs = rule.lhs;
        const auto& rhs = rule.rhs;

        if (rhs[0].empty()) {
            // If the right-hand side is empty, add the FOLLOW set of the left-hand side symbol to
            // the SELECT set.
            m_selectSet[i] = m_followSet[lhs];
        } else {
            size_t j = 0;
            for (; j < rhs.size(); ++j) {
                const Symbol& sym = rhs[j];
                if (isTerminal(sym)) {
                    // If the symbol is a terminal, add it to the SELECT set.
                    m_selectSet[i].insert(sym);
                    break;
                } else {
                    // If the symbol is a non-terminal, add the FIRST set of the symbol to the
                    // SELECT set.
                    bool hasNull = false;
                    for (const Symbol& s : m_firstSet[sym]) {
                        if (!s.empty()) {
                            m_selectSet[i].insert(s);
                        } else {
                            hasNull = true;
                        }
                    }

                    if (!hasNull) {
                        break;
                    }
                }
            }

            if (j == rhs.size()) {
                // If the FIRST set of all symbols in the right-hand side contains ε, add the
                // FOLLOW set of the left-hand side symbol to the SELECT set.
                m_selectSet[i] = m_followSet[lhs];
            }
        }
    }
}

void Syntax::printResults()
{
    // Print the FIRST set
    std::cout << "FIRST set:" << std::endl;
    for (const auto& entry : m_firstSet) {
        std::cout << entry.first << ": { ";
        for (const auto& s : entry.second) {
            if (s == "") {
                std::cout << "ε ";
            } else {
                std::cout << s << " ";
            }
        }
        std::cout << "}" << std::endl;
    }
    std::cout << std::endl;

    // Print the FOLLOW set
    std::cout << "FOLLOW set:" << std::endl;
    for (const auto& entry : m_followSet) {
        std::cout << entry.first << ": { ";
        for (const auto& s : entry.second) {
            std::cout << s << " ";
        }
        std::cout << "}" << std::endl;
    }
    std::cout << std::endl;

    // Print the SELECT set
    std::cout << "SELECT set:" << std::endl;
    for (size_t i = 0; i < m_productionRules.size(); ++i) {
        const ProductionRule& rule = m_productionRules[i];
        std::cout << std::format("{} -> ", rule.lhs);
        for (const auto& s : rule.rhs) {
            if (s.empty()) {
                std::cout << "ε";
            } else {
                std::cout << s;
            }
        }
        std::cout << ": { ";
        for (const auto& s : m_selectSet[i]) {
            if (s.empty()) {
                std::cout << "ε ";
            } else {
                std::cout << s << " ";
            }
        }
        std::cout << "}" << std::endl;
    }
    std::cout << std::endl;
}

void Syntax::clear()
{
    m_terminals.clear();
    m_nonTerminals.clear();
    m_firstSet.clear();
    m_followSet.clear();
    m_productionRules.clear();
    m_selectSet.clear();
}

}  // namespace PL0