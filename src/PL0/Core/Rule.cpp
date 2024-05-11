#include "PL0/Core/Rule.hpp"
#include <format>
#include <iostream>
#include <ranges>

namespace PL0
{
void RuleAnalyzer::addRule(const Symbol& lhs, const std::vector<Symbol>& rhs)
{
    // Add the rule to m_rules.
    Rule rule{lhs, rhs};
    m_rules.push_back(rule);

    ///////////////////////////////////////////
    // Collect all terminals and non-terminals
    ///////////////////////////////////////////

    /**
     * @note For the left-hand side symbol, it must be a non-terminal.
     */
    m_nonTerminals.insert(lhs);

    /**
     * @note For the right-hand side symbols, they can be terminals, non-terminals, or ε.
     */

    if (rhs[0] == EPSILON) {
        /**
         * @note ε does not belong to any set. Skip it.
         */
        return;
    }

    for (const Symbol& sym : rhs) {
        /**
         * @note If the symbol begins with A-Z, it is a non-terminal.
         *      Otherwise, it is a terminal.
         */
        if (std::isupper(sym[0])) {
            m_nonTerminals.insert(sym);
        } else {
            m_terminals.insert(sym);
        }
    }
}

void RuleAnalyzer::calcFirstSets()
{
    m_firstSet.clear();

    /**
     * @note Initialize the FIRST set.
     *      If x is a terminal symbol, then FIRST(x) = {x}.
     */
    for (const Symbol& sym : m_terminals) {
        m_firstSet[sym] = {sym};
    }
    for (const Symbol& sym : m_nonTerminals) {
        m_firstSet[sym] = {};
    }

    /**
     * @note To calculate the FIRST set of non-terminal X,
     *          1. Find all the rules that have X in the left-hand side.
     *          2. Calculate the FIRST set of the right-hand side symbols of these rules.
     *          3. Union these FIRST sets.
     *     However, the FIRST sets of some right-hand side symbols may have not been calculated.
     *     We need to keep iterating until no more symbols can be added to the FIRST set.
     */

    bool updated;
    do {
        updated = false;
        m_firstSetCache.clear();  // Discard the cache

        for (const auto& rule : m_rules) {
            // Calculate the FIRST set of the right-hand side symbols of the rule.
            auto [firstSet, allHasEpsilon] = calcFirstSetOfSyms(rule.rhs, 0, rule.rhs.size());

            // Union the FIRST sets.
            for (const Symbol& sym : firstSet) {
                auto [_, inserted] = m_firstSet[rule.lhs].insert(sym);
                if (inserted) {
                    updated = true;
                }
            }

            /**
             * @note Cache the FIRST set of each rule.
             *      If no FIRST set has been updated, the cache is valid.
             *      Otherwise, discard the cache and recalculate the FIRST set in the next
             *      iteration.
             */
            if (!updated) {
                m_firstSetCache.push_back({firstSet, allHasEpsilon});
            }
        }
    } while (updated);
}

void RuleAnalyzer::calcFollowSets()
{
    m_followSet.clear();

    for (const auto& sym : m_nonTerminals) {
        m_followSet[sym] = {};
    }

    /**
     * @note To calculate the FOLLOW set of non-terminal X,
     *          1. Add ENDSYM to the FOLLOW set of the begin symbol.
     *          2. Find all the rules that have X in the right-hand side.
     *          3. Calculate the FIRST set of the symbols after X in the right-hand side.
     *          4. Add the non-ε symbols in the FIRST set to the FOLLOW set of X.
     *          5. If the FIRST set contains ε, add the FOLLOW set of the left-hand side symbol.
     *     However, the FOLLOW sets of some symbols may have not been calculated.
     *     We need to keep iterating until no more symbols can be added to the FOLLOW set.
     */

    // Add # to the FOLLOW set of the begin symbol.
    m_followSet[m_beginSym].insert(ENDSYM);

    bool updated;
    do {
        updated = false;
        for (const auto& rule : m_rules) {
            const auto& lhs = rule.lhs;
            const auto& rhs = rule.rhs;

            for (size_t i = 0; i < rhs.size(); ++i) {
                const Symbol& rhsSym = rhs[i];  // X

                if (isNonTerminal(rhsSym)) {
                    // Calculate the FIRST set of the symbols after X in the right-hand side.
                    auto [firstSet, allHasEpsilon] = calcFirstSetOfSyms(rhs, i + 1, rhs.size());
                    for (const Symbol& sym : firstSet) {
                        // Add the non-ε symbols in the FIRST set to the FOLLOW set of X.
                        if (sym != EPSILON) {
                            auto [_, inserted] = m_followSet[rhsSym].insert(sym);
                            if (inserted) {
                                updated = true;
                            }
                        }
                    }

                    // If the FIRST set contains ε, add the FOLLOW set of the left-hand side symbol.
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

void RuleAnalyzer::calcSelectSets()
{
    calcFirstSets();
    calcFollowSets();

    m_selectSet.clear();
    m_selectSet.resize(m_rules.size());

    for (size_t i = 0; i < m_rules.size(); ++i) {
        const auto& lhs = m_rules[i].lhs;  // X
        const auto& rhs = m_rules[i].rhs;  // Y1Y2...Yk

        // 1. Get the FIRST set of Y1Y2...Yk.
        auto [firstSet, allHasEpsilon] = m_firstSetCache[i];
        // 2. Add the non-ε symbols in the FIRST set to the SELECT set of X.
        for (const Symbol& sym : firstSet) {
            if (sym != EPSILON) {
                m_selectSet[i].insert(sym);
            }
        }
        // 3. If all symbols in the FIRST set have ε, add the FOLLOW set of X to the SELECT set of
        // X.
        if (allHasEpsilon) {
            for (const Symbol& sym : m_followSet[lhs]) {
                m_selectSet[i].insert(sym);
            }
        }
    }

    m_firstSetCache.clear();
}

std::pair<std::set<Symbol>, bool> RuleAnalyzer::calcFirstSetOfSyms(const std::vector<Symbol>& syms,
                                                                   size_t beginIdx,
                                                                   size_t endIdx) const
{
    std::set<Symbol> firstSet;

    // If X -> ε, FIRST(X) = {ε}.
    if (syms[beginIdx] == EPSILON) {
        firstSet.insert(EPSILON);
        return {firstSet, true};
    }

    size_t i = beginIdx;
    for (; i < endIdx; ++i) {
        // If Y1, Y2, ..., Yi-1 => ε, then add First(Y1)-{ε}, First(Y2)-{ε}, ..., First(Yi) to
        // FIRST(X).
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
            // If there is a terminal, then add it to FIRST(X) and stop the loop.
            firstSet.insert(syms[i]);
            break;
        }
    }

    // If Yi => ε for all i = 1, 2, ..., k, then add ε to FIRST(X).
    bool allHasEpsilon = (i == endIdx);
    if (allHasEpsilon) {
        firstSet.insert(EPSILON);
    }

    return {firstSet, allHasEpsilon};
}

void RuleAnalyzer::printResults()
{
    // FIRST set
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

    // FOLLOW set
    std::cout << "FOLLOW set:" << std::endl;
    for (const auto& entry : m_followSet) {
        std::cout << entry.first << ": { ";
        for (const auto& s : entry.second) {
            std::cout << s << " ";
        }
        std::cout << "}" << std::endl;
    }
    std::cout << std::endl;

    // SELECT set
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

void RuleAnalyzer::clear()
{
    m_terminals.clear();
    m_nonTerminals.clear();
    m_firstSet.clear();
    m_followSet.clear();
    m_rules.clear();
    m_selectSet.clear();
}

}  // namespace PL0