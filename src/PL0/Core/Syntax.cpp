#include "PL0/Core/Syntax.hpp"
#include <format>
#include <iostream>
#include <ranges>

namespace PL0
{
void Syntax::addRule(const Symbol& lhs, const std::vector<Symbol>& rhs)
{
    Rule rule{lhs, rhs};
    m_rules.push_back(rule);

    ///////////////////////////////////////////
    // Collect all terminals and non-terminals
    ///////////////////////////////////////////

    // For the left-hand side symbol, it must be a non-terminal.
    m_nonTerminals.insert(lhs);

    // For the right-hand side symbols, they can be terminals, non-terminals, or ε.
    if (rhs[0] == EPSILON) {  // Skip ε
        return;
    }

    for (const Symbol& sym : rhs) {
        if (std::isupper(sym[0])) {
            // If the symbol begins with A-Z, it is a non-terminal.
            m_nonTerminals.insert(sym);
        } else {
            // Otherwise, it is a terminal.
            // e.g. id, +, (
            m_terminals.insert(sym);
        }
    }
}

void Syntax::calcFirstSets()
{
    m_firstSet.clear();

    //////////////////////////////////////////////////////
    // If X is a terminal symbol, then FIRST(X) = {X}.
    //////////////////////////////////////////////////////
    for (const Symbol& sym : m_terminals) {
        m_firstSet[sym] = {sym};
    }
    for (const Symbol& sym : m_nonTerminals) {
        m_firstSet[sym] = {};
    }

    // Keep iterating until no more symbols can be added to the FIRST set.
    bool updated;
    do {
        updated = false;
        m_firstSetCache.clear();

        for (const auto& rule : m_rules) {
            auto [firstSet, allHasEpsilon] = calcFirstSetOfSyms(rule.rhs, 0, rule.lhs.size());

            for (const Symbol& sym : firstSet) {
                auto [_, inserted] = m_firstSet[rule.lhs].insert(sym);
                if (inserted) {
                    updated = true;
                }
            }

            if (!updated) {
                m_firstSetCache.push_back({firstSet, allHasEpsilon});
            }
        }
    } while (updated);
}

void Syntax::calcFollowSets()
{
    m_followSet.clear();

    for (const auto& sym : m_nonTerminals) {
        m_followSet[sym] = {};
    }
    ///////////////////////////////////////////////////
    // Add # to the FOLLOW set of the begin symbol.
    ///////////////////////////////////////////////////
    m_followSet[m_beginSym].insert("#");

    // Keep iterating until no more symbols can be added to the FOLLOW set.
    bool updated;
    do {
        updated = false;
        for (const auto& rule : m_rules) {
            const auto& lhs = rule.lhs;
            const auto& rhs = rule.rhs;

            for (size_t i = 0; i < rhs.size(); ++i) {
                const Symbol& rhsSym = rhs[i];

                if (isNonTerminal(rhsSym)) {
                    // Add the FIRST set of the next symbol to the FOLLOW set of the current symbol.
                    auto [firstSet, allHasEpsilon] = calcFirstSetOfSyms(rhs, i + 1, rhs.size());
                    for (const Symbol& sym : firstSet) {
                        if (sym != EPSILON) {
                            auto [_, inserted] = m_followSet[rhsSym].insert(sym);
                            if (inserted) {
                                updated = true;
                            }
                        }
                    }

                    // If the FIRST set of all symbols in the right-hand side contains ε, add the
                    // FOLLOW set of the left-hand side symbol.
                    if (allHasEpsilon) {
                        for (const Symbol& sym : m_followSet[lhs]) {
                            auto [_, inserted] = m_followSet[rhsSym].insert(sym);
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

void Syntax::calcSelectSets()
{
    calcFirstSets();
    calcFollowSets();

    m_selectSet.clear();
    m_selectSet.resize(m_rules.size());

    for (size_t i = 0; i < m_rules.size(); ++i) {
        const auto& lhs = m_rules[i].lhs;
        const auto& rhs = m_rules[i].rhs;

        auto [firstSet, allHasEpsilon] = m_firstSetCache[i];
        for (const Symbol& sym : firstSet) {
            if (sym != EPSILON) {
                m_selectSet[i].insert(sym);
            }
        }
        if (allHasEpsilon) {
            for (const Symbol& sym : m_followSet[lhs]) {
                m_selectSet[i].insert(sym);
            }
        }
    }

    m_firstSetCache.clear();
}

std::pair<std::set<Symbol>, bool> Syntax::calcFirstSetOfSyms(const std::vector<Symbol>& syms,
                                                             size_t beginIdx, size_t endIdx) const
{
    std::set<Symbol> firstSet;

    //////////////////////////////////////////////////////////////////////////
    // If X -> ε, then add ε to FIRST(X).
    //////////////////////////////////////////////////////////////////////////
    if (syms[beginIdx] == EPSILON) {
        firstSet.insert(EPSILON);
        return {firstSet, true};
    }

    size_t i = beginIdx;
    for (; i < endIdx; ++i) {
        ////////////////////////////////////////////////////////////////////////////
        // If Y1, Y2, ..., Yi-1 => ε, then add First(Y1)-{ε}, First(Y2)-{ε}, ...,
        // First(Yi) to FIRST(X).
        ////////////////////////////////////////////////////////////////////////////
        if (isNonTerminal(syms[i])) {
            bool hasEpsilon = false;
            for (const Symbol& sym : m_firstSet.at(syms[i])) {
                if (sym != EPSILON) {
                    firstSet.insert(sym);
                } else {
                    hasEpsilon = true;
                }
            }

            if (!hasEpsilon) {
                break;
            }
        } else {
            //////////////////////////////////////////////////////////////////////////
            // If there is a terminal, then add it to FIRST(X) and stop the loop.
            //////////////////////////////////////////////////////////////////////////
            firstSet.insert(syms[i]);
            break;
        }
    }

    //////////////////////////////////////////////////////////////////
    // If Yi => ε for all i = 1, 2, ..., k, then add ε to FIRST(X).
    //////////////////////////////////////////////////////////////////
    bool allHasEpsilon = (i == endIdx);
    if (allHasEpsilon) {
        firstSet.insert(EPSILON);
    }
    return {firstSet, allHasEpsilon};
}

void Syntax::printResults()
{
    // Print the FIRST set
    std::cout << "FIRST set:" << std::endl;
    for (const auto& entry : m_firstSet) {
        std::cout << entry.first << ": { ";
        for (const auto& s : entry.second) {
            if (s == EPSILON) {
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
    for (size_t i = 0; i < m_rules.size(); ++i) {
        const Rule& rule = m_rules[i];
        std::cout << std::format("{} -> ", rule.lhs);
        for (const auto& s : rule.rhs) {
            if (s == EPSILON) {
                std::cout << "ε";
            } else {
                std::cout << s;
            }
        }
        std::cout << ": { ";
        for (const auto& s : m_selectSet[i]) {
            if (s == EPSILON) {
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
    m_rules.clear();
    m_selectSet.clear();
}

}  // namespace PL0