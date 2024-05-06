#pragma once
#include "Parser.hpp"

namespace PL0
{
class RecursiveDescentParser: public Parser
{
public:
    virtual void parse(const std::vector<Token>& tokens) override;

private:
    // <Exp> ::= [<AddOp>] <Term> { <AddOp> <Term> }
    void exp();

    // <Term> ::= <Factor> { <MulOp> <Factor> }
    void term();

    // <Factor> ::= <Identifier> | <Number> | '(' <Exp> ')'
    void factor();

    ////////////////////////////////////////////////////
    // Extensions
    ////////////////////////////////////////////////////

    // // <Condition> ::= <Exp> <RelOp> <Exp> | odd <Exp>
    // bool condition();

private:

    // <AddOp> ::= +|-
    bool isAddOp() {
        const Token& token = getCurToken();
        return token.type == TokenType::Operator &&
               (token.value == "+" || token.value == "-");
    }

    // <MulOp> ::= *|/
    bool isMulOp() {
        const Token& token = getCurToken();
        return token.type == TokenType::Operator &&
               (token.value == "*" || token.value == "/");
    }

    // <RelOp> ::= =|#|<|<=|>|>=
    bool isRelOp() {
        const Token& token = getCurToken();
        return token.type == TokenType::Operator &&
               (token.value == "=" || token.value == "#" || token.value == "<" ||
                token.value == "<=" || token.value == ">" || token.value == ">=");
    }

private:
    inline bool reachEnd() const
    {
        return m_curIndex >= m_tokens->size();
    }

    inline const Token& getCurToken() const
    {
        return m_tokens->at(m_curIndex);
    }

    inline void consume()
    {
        if (m_curIndex < m_tokens->size()) {
            m_curIndex++;
        }
    }
};
} // namespace PL0
