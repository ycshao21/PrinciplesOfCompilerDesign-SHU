#pragma once
#include "Action.hpp"
#include "Parser.hpp"
#include "Rule.hpp"

namespace PL0
{
constexpr int NULL_OFFSET = -999999999;  // The value does not need to be passed.

/**
 * @brief Semantic parser for PL/0 using LL(1) parsing.
 * @note This parser can only parse arithmetic expressions.
 */
class SemanticLL1Parser : public Parser
{
    /**
     * @note The prediction table for LL(1) parsing.
     *    < left-hand side, <symbol in SELECT set, right-hand side> >
     */
    using PredictionTable = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;
    /**
     * @note The offset table for non-terminal symbols.
     *   < non-terminal symbol, <symbol, offset> >
     */
    using IndexOffsetTable = std::map<Symbol, std::map<Symbol, int>>;
    /**
     * @note The action function type.
     * @note const std::vector<int>&: Operands for the action function.
     */
    using ActionFunc = std::function<int(const std::vector<int>&)>;

public:
    SemanticLL1Parser();

    /**
     * @brief Parse the given tokens.
     * @param tokens The tokens to parse.
     * @note Once there is a syntax or semantic error, the parsing process will stop immediately.
     */
    virtual void parse(const std::vector<Token>& tokens) override;

private:
    void initSyntax();

    /**
     * @brief Add a rule to the syntax analyzer.
     * @param lhs The left-hand side of the rule.
     * @param rhs The right-hand side of the rule.
     * @param indexOffset The offset to locate where the value of an non-terminal symbol should be
     *      passed to.
     */
    void addRule(const Symbol& lhs, const std::vector<Symbol>& rhs, int indexOffset = NULL_OFFSET);

    /**
     * @brief Set the action function for the given action symbol.
     * @param actionSym The action symbol. e.g. "10"
     * @param func The action function.
     */
    void setActionFunc(const std::string& actionSym, const ActionFunc& func);

    /**
     * @brief Generate the prediction table and the index offset table.
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