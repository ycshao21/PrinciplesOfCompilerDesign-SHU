#include "PL0/Core/Rule.hpp"
#include <format>
#include <iostream>
#include <ranges>

namespace PL0
{
void RuleAnalyzer::addRule(const Symbol& lhs, const std::vector<Symbol>& rhs)
{
    // 将规则添加到 m_rules 中。
    Rule rule{lhs, rhs};
    m_rules.push_back(rule);

    /////////////////////////////////////////////////////////////////
    // 将所有终结符和非终结符收集到 m_terminals 和 m_nonTerminals 中。
    /////////////////////////////////////////////////////////////////

    /**
     * @note 左侧一定是非终结符。
    */
    m_nonTerminals.insert(lhs);

    /**
     * @note 右侧的符号可能是终结符、非终结符或 ε。
     */

    if (rhs[0] == EPSILON) {
        /**
         * @note ε 不属于任何集合，跳过
        */
        return;
    }

    for (const Symbol& sym : rhs) {
        /**
         * @note 如果符号以 A-Z 开头，它是一个非终结符，否则是一个终结符。
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
     * @note 初始化 FIRST 集合。
     *      如果 x 是终结符，则 FIRST(x) = {x}。
    */
    for (const Symbol& sym : m_terminals) {
        m_firstSet[sym] = {sym};
    }
    for (const Symbol& sym : m_nonTerminals) {
        m_firstSet[sym] = {};
    }

    /**
     * @note 非终结符的 FIRST 集合的计算方法：
     *          1. 将所有非终结符的 FIRST 集合初始化为空。
     *          2. 计算每个规则右侧符号串的 FIRST 集合。
     *          3. 将这些 FIRST 集合的非空符号并入左侧符号的 FIRST 集合。
     *      然而，某些符号的 FIRST 集合可能尚未计算，需要不断循环，直到没有新的符号添加到 FIRST 集合。
    */

    bool updated;
    do {
        updated = false;
        m_firstSetCache.clear();  // 清空缓存

        for (const auto& rule : m_rules) {
            // 计算每个规则右侧符号串的 FIRST 集合。
            auto [firstSet, allHasEpsilon] = calcFirstSetOfSyms(rule.rhs, 0, rule.rhs.size());

            // 将 FIRST 集元素并入左侧符号的 FIRST 集合。
            for (const Symbol& sym : firstSet) {
                auto [_, inserted] = m_firstSet[rule.lhs].insert(sym);
                if (inserted) {
                    updated = true;
                }
            }

            /**
             * @note 缓存每个规则的 FIRST 集合。
             *      如果没有更新 FIRST 集合，缓存仍然有效；否则，丢弃缓存，并在下一次迭代中重新计算 FIRST 集合。
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
     * @note 非终结符的 FOLLOW 集合的计算方法：
     *          1. 将结束符号添加到起始符号的 FOLLOW 集合中。
     *          2. 找到所有右侧包含 X 的规则。
     *          3. 计算 X 后面的符号串的 FIRST 集合。
     *          4. 将 FIRST 集合中的非空符号添加到 X 的 FOLLOW 集合中。
     *          5. 如果 FIRST 集合包含空，将左侧符号的 FOLLOW 集合添加到 X 的 FOLLOW 集合中。
     *      然而，某些符号的 FOLLOW 集合可能尚未计算，需要不断循环，直到没有新的符号添加到 FOLLOW 集合。
    */

    // 将结束符号添加到起始符号的 FOLLOW 集合中。
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
                    // 计算 X 后面的符号串的 FIRST 集合。
                    auto [firstSet, allHasEpsilon] = calcFirstSetOfSyms(rhs, i + 1, rhs.size());
                    for (const Symbol& sym : firstSet) {
                        // 将 FIRST 集合中的非空符号添加到 X 的 FOLLOW 集合中。
                        if (sym != EPSILON) {
                            auto [_, inserted] = m_followSet[rhsSym].insert(sym);
                            if (inserted) {
                                updated = true;
                            }
                        }
                    }

                    // 如果 FIRST 集合包含空，将左侧符号的 FOLLOW 集合添加到 X 的 FOLLOW 集合中。
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

        // 1. 计算 Y1Y2...Yk 的 FIRST 集。
        auto [firstSet, allHasEpsilon] = m_firstSetCache[i];
        // 2. 将 FIRST 集中的非空符号添加到 X 的 SELECT 集合中。
        for (const Symbol& sym : firstSet) {
            if (sym != EPSILON) {
                m_selectSet[i].insert(sym);
            }
        }
        // 3. 如果 FIRST 集中的所有符号都包含 ε，将 X 的 FOLLOW 集合添加到 X 的 SELECT 集合中。
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

    // 若 X -> ε，则 FIRST(X) = {ε}。
    if (syms[beginIdx] == EPSILON) {
        firstSet.insert(EPSILON);
        return {firstSet, true};
    }

    size_t i = beginIdx;
    for (; i < endIdx; ++i) {
        // 如果 Y1, Y2, ..., Yi-1 => ε，则将 First(Y1)-{ε}、First(Y2)-{ε}、...、First(Yi) 添加到
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
            // 如果 Yi 是终结符，则将其添加到 FIRST(X) 中并停止循环。
            firstSet.insert(syms[i]);
            break;
        }
    }

    // 如果 Y1, Y2, ..., Yk => ε，则将 ε 添加到 FIRST(X) 中。
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