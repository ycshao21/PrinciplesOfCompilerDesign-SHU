#pragma once
#include "Parser.hpp"

namespace PL0
{
/**
 * @brief Syntax parser for PL/0 using recursive descent parsing.
 * @note This parser can only parse arithmetic expressions.
*/
class RecursiveDescentParser: public Parser
{
public:
    /**
     * @brief Parse the given tokens.
     * @param tokens The tokens to parse.
     * @note Once there is a syntax error, the parsing process will stop immediately.
    */
    virtual void parse(const std::vector<Token>& tokens) override;

private:
    // <Exp> ::= [<AddOp>] <Term> { <AddOp> <Term> }
    void exp();

    // <Term> ::= <Factor> { <MulOp> <Factor> }
    void term();

    // <Factor> ::= <Identifier> | <Number> | '(' <Exp> ')'
    void factor();

private:

    // <AddOp> ::= +|-
    bool isAddOp() const;

    // <MulOp> ::= *|/
    bool isMulOp() const;

private:
    /**
     * @brief Move to the next token (if not reach the end).
    */
    inline void consume()
    {
        if (m_curIndex < m_tokens->size()) {
            m_curIndex++;
        }
    }

    /**
     * @return True if all tokens are consumed, false otherwise.
    */
    inline bool reachEnd() const
    {
        return m_curIndex >= m_tokens->size();
    }

    /**
     * @return The current token.
    */
    inline const Token& getCurToken() const
    {
        return m_tokens->at(m_curIndex);
    }

private:
    std::unique_ptr<std::vector<Token>> m_tokens = nullptr;  // The tokens to parse
    size_t m_curIndex = 0;  // The index of the current token
};
} // namespace PL0
