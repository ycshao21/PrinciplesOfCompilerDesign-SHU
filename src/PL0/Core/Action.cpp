#include "PL0/Core/Action.hpp"
#include "PL0/Utils/Exception.hpp"
#include "PL0/Utils/Reporter.hpp"
#include <format>

namespace PL0
{

namespace Action
{
int print(const std::vector<int>& operands)
{
    if (operands.empty()) {
        throw SemanticError("No operand to print.");
    }
    if (operands.size() > 1) {
        throw SemanticError("Too many operands to print.");
    }
    int ans = operands[0];
    Reporter::info(std::format("Ans = {}", ans));
    return ans;
}

int assign(const std::vector<int>& operands)
{
    if (operands.empty()) {
        throw SemanticError("No operand for assignment.");
    }
    if (operands.size() > 1) {
        throw SemanticError("Too many operands for assignment.");
    }
    return operands[0];
}

int opposite(const std::vector<int>& operands)
{
    if (operands.empty()) {
        throw SemanticError("No operand for opposite.");
    }
    if (operands.size() > 1) {
        throw SemanticError("Too many operands for opposite.");
    }
    return -operands[0];
}

int add(const std::vector<int>& operands)
{
    if (operands.size() < 2) {
        throw SemanticError("Too few operands for addition.");
    }
    if (operands.size() > 2) {
        throw SemanticError("Too many operands for addition.");
    }

    return operands[0] + operands[1];
}

int sub(const std::vector<int>& operands)
{
    if (operands.size() < 2) {
        throw SemanticError("Too few operands for subtraction.");
    }
    if (operands.size() > 2) {
        throw SemanticError("Too many operands for subtraction.");
    }
    return operands[0] - operands[1];
}

int mul(const std::vector<int>& operands)
{
    if (operands.size() < 2) {
        throw SemanticError("Too few operands for multiplication.");
    }
    if (operands.size() > 2) {
        throw SemanticError("Too many operands for multiplication.");
    }
    return operands[0] * operands[1];
}

int div(const std::vector<int>& operands)
{
    if (operands.size() < 2) {
        throw SemanticError("Too few operands for division.");
    }
    if (operands.size() > 2) {
        throw SemanticError("Too many operands for division.");
    }
    if (operands[1] == 0) {
        throw SemanticError("Division by zero.");
    }
    return operands[0] / operands[1];
}
}  // namespace Action

}  // namespace PL0