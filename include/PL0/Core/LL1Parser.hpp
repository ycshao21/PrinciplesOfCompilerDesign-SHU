#pragma once
#include "Parser.hpp"
#include "Rule.hpp"
#include "Symbol.hpp"

namespace PL0
{
/**
 * @brief Syntax parser for PL/0 using LL(1) parsing.
 * @note This parser can only parse arithmetic expressions.
*/
class LL1Parser : public Parser
{
    /**
     * @note The prediction table for LL(1) parsing.
     *     < left-hand side, <symbol in SELECT set, right-hand side> >
    */
    using PredictionTable = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;

public:
    LL1Parser();

    /**
     * @brief Parse the given tokens.
     * @param tokens The tokens to parse.
     * @note Once there is a syntax error, the parsing process will stop immediately.
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