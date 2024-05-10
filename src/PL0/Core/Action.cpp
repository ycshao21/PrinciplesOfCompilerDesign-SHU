#include "PL0/Core/Action.hpp"
#include "PL0/Utils/Reporter.hpp"
#include <format>


namespace PL0
{
int printAns(const std::vector<Operand>& operands) {
    int ans = operands[0].second.value();
    Reporter::info(std::format("Ans = {}", ans));
    return ans;
}

int assign(const std::vector<Operand>& operands) {
    return operands[0].second.value();
}

int opposite(const std::vector<Operand>& operands) {
    return -operands[0].second.value();
}

int add(const std::vector<Operand>& operands) {
    return operands[0].second.value() + operands[1].second.value();
}

int sub(const std::vector<Operand>& operands) {
    return operands[0].second.value() - operands[1].second.value();
}

int mul(const std::vector<Operand>& operands) {
    return operands[0].second.value() * operands[1].second.value();
}

int div(const std::vector<Operand>& operands) {
    if (operands[1].second.value() == 0) {
        throw std::runtime_error("Semantic error: Division by zero.");
    }
    return operands[0].second.value() / operands[1].second.value();
}
}  // namespace PL0