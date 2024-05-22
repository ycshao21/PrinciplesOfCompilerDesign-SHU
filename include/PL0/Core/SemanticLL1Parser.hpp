#pragma once
#include "Action.hpp"
#include "Parser.hpp"
#include "Rule.hpp"

namespace PL0
{
constexpr int NULL_OFFSET = -999999999;  // The value does not need to be passed.

/**
 * @brief 使用预测分析表实现的 PL/0 语义分析器。
 * @note 该分析器只能解析算术表达式。
 */
class SemanticLL1Parser : public Parser
{
    /**
     * @note 预测分析表 <产生式左侧, <SELECT 集元素, 产生式右侧> >
    */
    using PredictionTable = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;
    /**
     * @note 非终结符的偏移表 <非终结符, <终结符, 偏移> >
    */
    using IndexOffsetTable = std::map<Symbol, std::map<Symbol, int>>;
    /**
     * @note 动作函数类型
     * @note const std::vector<int>&: 动作函数的操作数
    */
    using ActionFunc = std::function<int(const std::vector<int>&)>;

public:
    SemanticLL1Parser();

    /**
     * @brief 解析给定的词。
     * @param tokens 要解析的词。
     * @note 一旦出现语法或语义错误，解析过程将立即停止。
    */
    virtual void parse(const std::vector<Token>& tokens) override;

private:
    void initSyntax();

    /**
     * @brief 添加一条规则。
     * @param lhs 规则的左侧。
     * @param rhs 规则的右侧。
     * @param indexOffset 用于定位非终结符的值应该传递到哪里的偏移量。
    */
    void addRule(const Symbol& lhs, const std::vector<Symbol>& rhs, int indexOffset = NULL_OFFSET);

    /**
     * @brief 为给定的动作符号设置动作函数。
     * @param actionSym 动作符号。例如 "10"
     * @param func 动作函数。
    */
    void setActionFunc(const std::string& actionSym, const ActionFunc& func);

    /**
     * @brief 生成预测分析表和偏移表。
    */
    void generateTables();

private:
    void printPredictionTable();
    void printState(const std::vector<Element>& analysisStack,
                    const std::vector<std::string>& inputStack);

private:
    RuleAnalyzer m_analyzer;

    PredictionTable m_predictionTable;
    IndexOffsetTable m_indexOffsetTable;
    std::map<std::string, ActionFunc> m_actionFuncs;

    std::vector<std::vector<Symbol>> m_rhsWithActions;
    std::vector<int> m_indexOffsets;
};

}  // namespace PL0