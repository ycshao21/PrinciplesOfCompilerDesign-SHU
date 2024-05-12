#pragma once
#include "Symbol.hpp"
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>

namespace PL0
{
/**
 * @brief A rule in the form of X -> Y1 Y2 ... Yn.
 */
struct Rule
{
    Symbol lhs;               // X
    std::vector<Symbol> rhs;  // Y1, Y2, ..., Yn
};

/**
 * @brief A class to analyze rules and calculate FIRST, FOLLOW, and SELECT sets.
 */
class RuleAnalyzer
{
public:
    /**
     * @brief Add a rule to the analyzer.
     * @param lhs The left-hand side symbol.
     * @param rhs The right-hand side symbols.
     * @note lhs must be a non-terminal symbol,
     *      rhs must be a sequence of terminal and non-terminal symbols.
     *      Actions are not supported, please remove them before calling this function.
     */
    void addRule(const Symbol& lhs, const std::vector<Symbol>& rhs);

    /**
     * @brief Calculate SELECT sets for all rules.
     */
    void calcSelectSets();

    /**
     * @brief Set the begin symbol of the syntax.
     * @param sym The begin symbol.
     * @note The begin symbol must be a non-terminal symbol that appears in the left-hand side of a
     *      rule.
     */
    inline void setBeginSym(const Symbol& sym)
    {
        m_beginSym = sym;
    }

    /**
     * @return The begin symbol of the syntax.
     */
    inline const Symbol& getBeginSym() const
    {
        return m_beginSym;
    }

    /**
     * @return All the rules added to the analyzer.
     */
    inline const std::vector<Rule>& getRules() const
    {
        return m_rules;
    }

    /**
     * @param ruleIndex The index of the rule.
     * @return The SELECT set of a rule.
     * @note The index will not be checked, please make sure it is valid.
     */
    inline const std::set<Symbol>& getSelectSet(size_t ruleIndex) const
    {
        return m_selectSet[ruleIndex];
    }

    /**
     * @return Whether the symbol is a non-terminal.
     */
    inline bool isNonTerminal(const Symbol& sym) const
    {
        return m_nonTerminals.find(sym) != m_nonTerminals.end();
    }

    /**
     * @return Whether the symbol is a terminal.
     */
    inline bool isTerminal(const Symbol& sym) const
    {
        return m_terminals.find(sym) != m_terminals.end();
    }

    /**
     * @brief Reset the analyzer.
     */
    void clear();

    void printResults();

private:
    /**
     * @brief Calculate the FIRST set of a sequence of symbols.
     * @param syms The sequence of symbols.
     * @param beginIdx The index of the first symbol.
     * @param endIdx The next index after the last symbol.
     * @return The FIRST set of the sequence and whether all symbols have epsilon in their FIRST
     * set. <FirstSetOfsyms, allHasEpsilon>
     */
    std::pair<std::set<Symbol>, bool> calcFirstSetOfSyms(const std::vector<Symbol>& syms,
                                                         size_t beginIdx, size_t endIdx) const;

    /**
     * @brief Calculate the FIRST set of each symbol (including terminals and non-terminals).
     * @note The result will be saved in m_firstSet.
     * @note The FIRST set of each rule will be stored temporarily in m_firstSetCache for SELECT set
     *      calculation.
     */
    void calcFirstSets();
    /**
     * @brief Calculate the FOLLOW set of each non-terminal symbol.
     * @note The result will be saved in m_followSet.
     */
    void calcFollowSets();

private:
    Symbol m_beginSym;                // The begin symbol of the syntax.
    std::set<Symbol> m_terminals;     // All terminal symbols.
    std::set<Symbol> m_nonTerminals;  // All non-terminal symbols.

    /**
     * @note m_rules and m_selectSet are one-to-one correspondence.
     * @note Use the index of the rule to access the SELECT set.
     */
    std::vector<Rule> m_rules;                  // All rules added to the analyzer.
    std::vector<std::set<Symbol>> m_selectSet;  // The SELECT set of each rule.

    std::map<Symbol, std::set<Symbol>> m_firstSet;   // The FIRST set of each symbol.
    std::map<Symbol, std::set<Symbol>> m_followSet;  // The FOLLOW set of each non-terminal symbol.

    /**
     * @brief The first set of each rule.
     * @return <FirstSetOfRule, allHasEpsilon>
     * @note This is used to accelerate the calculation of SELECT sets.
     */
    mutable std::vector<std::pair<std::set<Symbol>, bool>> m_firstSetCache;
};

}  // namespace PL0
