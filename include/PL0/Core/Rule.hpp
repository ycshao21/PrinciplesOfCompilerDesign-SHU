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
 * @brief X -> Y1 Y2 ... Yn 形式的规则。
 */
struct Rule
{
    Symbol lhs;               // X
    std::vector<Symbol> rhs;  // Y1, Y2, ..., Yn
};

/**
 * @brief 用于分析规则并计算 FIRST、FOLLOW 和 SELECT 集合的类。
 */
class RuleAnalyzer
{
public:
    /**
     * @brief 添加一条规则。
     * @param lhs 规则的左侧。
     * @param rhs 规则的右侧。
     * @note lhs 必须是一个非终结符，
     *     rhs 必须是一个终结符和非终结符符号的序列。
     *     不允许出现动作符号，请在调用该函数之前移除它们。
    */
    void addRule(const Symbol& lhs, const std::vector<Symbol>& rhs);

    /**
     * @brief 计算所有规则的 SELECT 集合。
     */
    void calcSelectSets();

    /**
     * @brief 设置语法的开始符号。
     * @param sym 开始符号。
     * @note 开始符号必须是一个出现在规则左侧的非终结符。
    */
    inline void setBeginSym(const Symbol& sym)
    {
        m_beginSym = sym;
    }

    /**
     * @return 语法的开始符号。
    */
    inline const Symbol& getBeginSym() const
    {
        return m_beginSym;
    }

    /**
     * @return 所有添加到分析器的规则。
    */
    inline const std::vector<Rule>& getRules() const
    {
        return m_rules;
    }

    /**
     * @param ruleIndex 规则的索引。
     * @return 规则的 SELECT 集合。
     * @note 索引不会被检查，请确保它是有效的。
    */
    inline const std::set<Symbol>& getSelectSet(size_t ruleIndex) const
    {
        return m_selectSet[ruleIndex];
    }

    /**
     * @brief 判断符号是否是非终结符。
    */
    inline bool isNonTerminal(const Symbol& sym) const
    {
        return m_nonTerminals.find(sym) != m_nonTerminals.end();
    }

    /**
     * @brief 判断符号是否是终结符。
    */
    inline bool isTerminal(const Symbol& sym) const
    {
        return m_terminals.find(sym) != m_terminals.end();
    }

    /**
     * @brief 重置分析器。
    */
    void clear();

    void printResults();

private:
    /**
     * @brief 计算符号串的 FIRST 集合。
     * @param syms 符号串。
     * @param beginIdx 第一个符号的索引。
     * @param endIdx 最后一个符号的下一个索引。
     * @return <符号串的 FIRST 集合, 是否所有符号的 FIRST 集合都包含 epsilon>
    */
    std::pair<std::set<Symbol>, bool> calcFirstSetOfSyms(const std::vector<Symbol>& syms,
                                                         size_t beginIdx, size_t endIdx) const;

    /**
     * @brief 计算每个符号（包括终结符和非终结符）的 FIRST 集合。
     * @note 结果将保存在 m_firstSet 中。
     * @note 每个规则的 FIRST 集合将临时存储在 m_firstSetCache 中，以便计算 SELECT 集合。
    */
    void calcFirstSets();
    /**
     * @brief 计算每个非终结符的 FOLLOW 集合。
     * @note 结果将保存在 m_followSet 中。
    */
    void calcFollowSets();

private:
    Symbol m_beginSym;                // 起始符号。
    std::set<Symbol> m_terminals;     // 所有终结符。
    std::set<Symbol> m_nonTerminals;  // 所有非终结符。

    /**
     * @note m_rules 和 m_selectSet 一一对应，使用规则的索引访问 SELECT 集合。
    */
    std::vector<Rule> m_rules;                  // 所有规则。
    std::vector<std::set<Symbol>> m_selectSet;  // 所有规则的 SELECT 集合。

    std::map<Symbol, std::set<Symbol>> m_firstSet;   // 每个符号的 FIRST 集合。
    std::map<Symbol, std::set<Symbol>> m_followSet;  // 每个非终结符的 FOLLOW 集合。

    /**
     * @brief 每个规则的 FIRST 集合。
     * @return <规则的 FIRST 集合, 是否所有符号的 FIRST 集合都包含 epsilon>
     * @note 用于加速计算 SELECT 集合。
    */
    mutable std::vector<std::pair<std::set<Symbol>, bool>> m_firstSetCache;
};

}  // namespace PL0
