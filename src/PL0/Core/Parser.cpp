// #include "PL0/Core/Parser.hpp"
// #include "PL0/Utils/Reporter.hpp"

// namespace PL0
// {
// void Parser::parse(const std::vector<Token>& tokens)
// {
//     bool success = true;
//     success = expression();

//     if (success) {
//         Reporter::info("Syntax check passed!");
//     } else {
//         Reporter::error("Syntax check failed!");
//     }
// }

// bool Parser::expression()
// {
//     bool success = true;
//     // [+|-]
//     addOp();

//     // <Term>
//     success = term();
//     if (!success) {
//         // Reporter::error(std::format("Expecting a term, but got '{}'", getCurrentToken().value));
//         return false;
//     }

//     // { <AddOp> <Term> }
//     while (addOp()) {
//         success = term();
//         if (!success) {
//             // Reporter::error(std::format("Expecting a term, but got '{}'", getCurrentToken().value));
//             return false;
//         }
//     }
// }


// bool Parser::term() {
//     bool success = true;
//     // <Factor>
//     success = factor();
//     if (!success) {
//         Reporter::error(std::format("Expecting a factor, but got '{}'", getCurrentToken().value));
//         return false;
//     }

//     // { <MulOp> <Factor> }
//     while (mulOp()) {
//         success = factor();
//         if (!success) {
//             Reporter::error(std::format("Expecting a factor, but got '{}'", getCurrentToken().value));
//             return false;
//         }
//     }
// }

// bool addOp() {
//     const auto& token = getCurrentToken();
//     if (token.type == TokenType::Operator && (token.value == "+" || token.value == "-")) {
//         forward();
//         return true;
//     }
//     return false;
// }

// bool Parser::isMatched(TokenType type, const std::string& value)
// {
//     if (m_curIndex >= m_tokens.size()) {
//         throw std::runtime_error("Cannot go forward from the last token.");
//     }

//     const Token& token = getCurrentToken();
//     // if (token.type == type && (value.empty() || token.value == value)) {
//     //     forward();
//     //     return true;
//     // }
//     // return false;
//     return token.type == type && (value.empty() || token.value == value);
// }

// }  // namespace PL0