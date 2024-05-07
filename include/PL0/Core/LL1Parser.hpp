#pragma once
#include "Parser.hpp"
#include "Syntax.hpp"

namespace PL0
{
class LL1Parser : public Parser
{
    using Table = std::map<Symbol, std::map<Symbol, std::vector<Symbol>>>;

public:
    LL1Parser();

    virtual void parse(const std::vector<Token>& tokens) override;

private:
    void generatePredictionTable();

    void printTable();

private:
    Syntax m_syntax;
    Table m_table;
};

}  // namespace PL0