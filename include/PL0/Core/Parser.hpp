// #pragma once
// #include "PL0/Core/Token.hpp"
// #include <vector>

// namespace PL0
// {
// class Parser
// {
// public:
//     Parser() = default;
//     ~Parser() = default;

//     void parse(const std::vector<Token>& tokens);

// private:
//     // // <Program> ::= <Block>.
//     // bool parseProgram();

//     // // <Block> ::= [<ConstDeclarationPart>] [<VarDeclarationPart>] [<ProcedureDeclarationPart>] <Statement>
//     // bool parseBlock();

//     // <Exp> ::= [+|-] <Term> { <AddOp> <Term> }
//     bool expression();

//     // <Term> ::= <Factor> { <MulOp> <Factor> }
//     bool term();

//     // <Factor> ::= <Identifier> | <Number> | ( <Exp> )
//     bool factor();

//     // <AddOp> ::= +|-
//     bool addOp();

//     // <MulOp> ::= *|/
//     bool mulOp();

//     // <RelOp> ::= =|#|<|<=|>|>=
//     bool relOp();

// private:
//     inline const Token& getCurrentToken() const
//     {
//         return m_tokens[m_curIndex];
//     }

//     inline void forward()
//     {
//         m_curIndex++;
//     }

//     inline void backward()
//     {
//         // if (m_curIndex == 0) {
//         //     throw std::runtime_error("Cannot go backward from the first token.");
//         // }
//         m_curIndex--;
//     }

//     Token consume(TokenType tokenType, const std::string& value);

//     bool isMatched(TokenType type, const std::string& value = "");

//     template <typename... Args>
//     bool isMatched(TokenType type, const std::string& value, Args... values)
//     {
//         if (isMatched(type, value)) {
//             return true;
//         }
//         return isMatched(type, std::forward<Args>(values)...);
//     }

// private:
//     std::vector<Token> m_tokens = {};
//     std::size_t m_curIndex = 0;
//     std::vector<> m_requirements {}
// };

// }  // namespace PL0