#pragma once
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include "Symbol.hpp"

namespace PL0
{
struct Rule
{
    // lhs -> rhs
    // e.g. X -> Y1 Y2 ... Yn,
    // where X is the left-hand side symbol, and Y1, Y2, ..., Yn are the right-hand side symbols.
    Symbol lhs;
    std::vector<Symbol> rhs;
};

class Syntax
{
public:
    void addRule(const Symbol& lhs, const std::vector<Symbol>& rhs);
    void calcSelectSets();

    inline void setBeginSym(const Symbol& sym)
    {
        m_beginSym = sym;
    }

    inline const Symbol& getBeginSym() const
    {
        return m_beginSym;
    }

    inline const std::vector<Rule>& getRules() const
    {
        return m_rules;
    }

    inline const std::set<Symbol>& getFirstSet(const Symbol& sym) const
    {
        return m_firstSet.at(sym);
    }

    inline const std::set<Symbol>& getFollowSet(const Symbol& sym) const
    {
        return m_followSet.at(sym);
    }

    inline const std::set<Symbol>& getSelectSet(size_t ruleIndex) const
    {
        return m_selectSet[ruleIndex];
    }

    inline bool isNonTerminal(const Symbol& sym) const
    {
        return m_nonTerminals.find(sym) != m_nonTerminals.end();
    }

    inline bool isTerminal(const Symbol& sym) const
    {
        return m_terminals.find(sym) != m_terminals.end();
    }

    void clear();

    void printResults();

private:
    // Return <FirstSetOfsyms, allHasEpsilon>
    std::pair<std::set<Symbol>, bool> calcFirstSetOfSyms(const std::vector<Symbol>& syms,
                                                         size_t beginIdx, size_t endIdx) const;

    void calcFirstSets();
    void calcFollowSets();

private:
    Symbol m_beginSym;
    std::set<Symbol> m_terminals;
    std::set<Symbol> m_nonTerminals;
    std::vector<Rule> m_rules;
    std::map<Symbol, std::set<Symbol>> m_firstSet;
    std::map<Symbol, std::set<Symbol>> m_followSet;
    std::vector<std::set<Symbol>> m_selectSet;

    mutable std::vector<std::pair<std::set<Symbol>, bool>> m_firstSetCache;
};

}  // namespace PL0
