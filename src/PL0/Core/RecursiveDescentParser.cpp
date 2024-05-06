#include "PL0/Core/RecursiveDescentParser.hpp"
#include "PL0/Utils/Reporter.hpp"

namespace PL0
{
void RecursiveDescentParser::parse(const std::vector<Token>& tokens)
{
    m_tokens = std::make_unique<std::vector<Token>>(tokens);
    try {
        exp();
        if (!reachEnd()) {
            throw std::runtime_error(
                std::format("Syntax error! Expecting end of expression, but got '{}' instead.",
                            getCurToken().value));
        }
        Reporter::info("Syntax correct.");
    } catch (const std::exception& e) {
        Reporter::error(e.what());
    }
    m_tokens.reset();
}

// <Exp> ::= [<AddOp] <Term> { <AddOp> <Term> }
void RecursiveDescentParser::exp()
{
    // [<AddOp>]
    if (!reachEnd() && isAddOp()) {
        consume();
    }

    // <Term>
    term();

    // { <AddOp> <Term> }
    while (!reachEnd() && isAddOp()) {
        consume();
        term();
    }
}

// <Term> ::= <Factor> { <MulOp> <Factor> }
void RecursiveDescentParser::term()
{
    // <Factor>
    factor();

    // { <MulOp> <Factor> }
    while (!reachEnd() && isMulOp()) {
        consume();
        factor();
    }
}

// <Factor> ::= <Identifier> | <Number> | '(' <Exp> ')'
void RecursiveDescentParser::factor()
{
    if (!reachEnd() &&
        (getCurToken().type == TokenType::Identifier || getCurToken().type == TokenType::Number)) {
        consume();
    } else if (!reachEnd() && getCurToken().type == TokenType::Delimiter &&
               getCurToken().value == "(") {
        consume();
        exp();
        if (getCurToken().type != TokenType::Delimiter || getCurToken().value != ")") {
            throw std::runtime_error(
                std::format("Syntax error! Expecting ')' after expression, but got '{}' instead.",
                            getCurToken().value));
        }
        consume();
    } else {
        if (reachEnd()) {
            throw std::runtime_error(
                "Syntax error! Expecting identifier, number or '(', but reach end of expression.");
        } else {
            throw std::runtime_error(std::format(
                "Syntax error! Expecting identifier, number or '(', but got '{}' instead.",
                getCurToken().value));
        }
    }
}

}  // namespace PL0