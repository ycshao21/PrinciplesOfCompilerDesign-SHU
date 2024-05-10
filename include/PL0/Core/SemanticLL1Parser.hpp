#pragma once
#include "Parser.hpp"
#include "Syntax.hpp"
#include "Action.hpp"
#include <stack>

namespace PL0
{
class SemanticLL1Parser : public Parser
{
    using PredictionTable = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;
    using IndexOffsetTable = std::map<Symbol, std::map<Symbol, int>>;

public:
    SemanticLL1Parser();

    virtual void parse(const std::vector<Token>& tokens) override;

private:
    void addRule(const Symbol& lhs, const std::vector<Symbol>& rhs, int indexOffset = -99999);
    void setAction(const std::string& index,
                            const std::function<int(const std::vector<Operand>&)>& func,
                            const std::vector<std::string>& operandNames);

    void generateTables();

    void printTable();
    void printState(const std::vector<Element>& analysisStack,
                    const std::vector<std::string>& restInput);

private:
    Syntax m_syntax;
    std::vector<std::vector<Symbol>> m_rhsWithActions;

    PredictionTable m_predictionTable;
    IndexOffsetTable m_indexOffsetTable;  // Locate where the value of an non-terminal symbol should
                                          // be passed to.

    std::vector<int> m_indexOffsets;
    std::map<std::string, Action> m_actions;
};

}  // namespace PL0