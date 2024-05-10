#pragma once
#include "Action.hpp"
#include "Parser.hpp"
#include "Rule.hpp"

namespace PL0
{
constexpr int NO_VALUE = -999999999;

class SemanticLL1Parser : public Parser
{
    using PredictionTable = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;
    using IndexOffsetTable = std::map<Symbol, std::map<Symbol, int>>;
    using ActionFunc = std::function<int(const std::vector<int>&)>;

public:
    SemanticLL1Parser();

    virtual void parse(const std::vector<Token>& tokens) override;

private:
    void initSyntax();

    void addRule(const Symbol& lhs, const std::vector<Symbol>& rhs, int indexOffset = NO_VALUE);
    void setActionFunc(const std::string& index, const ActionFunc& func);

    void generateTables();

private:
    void printPredictionTable();
    void printState(const std::vector<Element>& analysisStack,
                    const std::vector<std::string>& inputStack);

private:
    RuleAnalyzer m_analyzer;

    std::vector<std::vector<Symbol>> m_rhsWithActions;
    std::vector<int> m_indexOffsets;

    PredictionTable m_predictionTable;
    IndexOffsetTable m_indexOffsetTable;  // Locate where the value of an non-terminal symbol should
                                          // be passed to.
    std::map<std::string, ActionFunc> m_actionFuncs;
};

}  // namespace PL0