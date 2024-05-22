#pragma once
#include "Parser.hpp"
#include "Rule.hpp"
#include "Symbol.hpp"

namespace PL0
{
/**
 * @brief 使用预测分析表实现的 PL/0 语法分析器。
*/
class LL1Parser : public Parser
{
    /**
     * @note 预测分析表 <产生式左侧, <SELECT 集元素, 产生式右侧> >
    */
    using PredictionTable = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;

public:
    LL1Parser();

    /**
     * @brief 解析给定的词。
     * @param tokens 要解析的词。
     * @note 一旦出现语法错误，解析过程将立即停止。
    */
    virtual void parse(const std::vector<Token>& tokens) override;

private:
    void initSyntax();
    void generatePredictionTable();

private:
    void printPredictionTable();
    void printState(const std::vector<Symbol>& analysisStack, const std::vector<Symbol>& inputStack);

private:
    RuleAnalyzer m_analyzer;
    PredictionTable m_predictionTable;
};

}  // namespace PL0