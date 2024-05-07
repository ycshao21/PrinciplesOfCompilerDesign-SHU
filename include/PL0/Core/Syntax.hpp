#pragma once
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>

namespace PL0
{
using Symbol = std::string;

struct ProductionRule
{
    Symbol lhs;
    std::vector<Symbol> rhs;
};

class Syntax
{
public:
    void addProductionRule(const Symbol& lhs, const std::vector<Symbol>& rhs);
    void calcSelectSet();

    inline void setBeginSym(const Symbol& sym)
    {
        m_beginSym = sym;
    }
    inline const Symbol& getBeginSym() const
    {
        return m_beginSym;
    }

    inline const std::vector<ProductionRule>& getProductionRules() const
    {
        return m_productionRules;
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
    void calcFirstSet();
    void calcFollowSet();

private:
    Symbol m_beginSym;
    std::set<Symbol> m_terminals;
    std::set<Symbol> m_nonTerminals;
    std::map<Symbol, std::set<Symbol>> m_firstSet;
    std::map<Symbol, std::set<Symbol>> m_followSet;
    std::vector<ProductionRule> m_productionRules;
    std::vector<std::set<Symbol>> m_selectSet;
};

}  // namespace PL0
