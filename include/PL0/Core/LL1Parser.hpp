#pragma once
#include "Parser.hpp"
#include "RuleAnalyzer.hpp"
#include "Symbol.hpp"

namespace PL0
{
class LL1Parser : public Parser
{
    using PredictionTable = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;

public:
    LL1Parser();

    virtual void parse(const std::vector<Token>& tokens) override;

private:
    void initSyntax();
    void generatePredictionTable();

private:
    void printPredictionTable();
    void printState(const std::vector<Symbol>& analysisStack, const std::vector<Symbol>& restInput);

private:
    RuleAnalyzer m_analyzer;
    PredictionTable m_predictionTable;
};

}  // namespace PL0