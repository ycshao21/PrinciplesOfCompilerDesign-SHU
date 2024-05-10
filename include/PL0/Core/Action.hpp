#pragma once
#include <functional>
#include <optional>
#include <string>

namespace PL0
{
// <SymbolName, Value>
using Operand = std::pair<std::string, std::optional<int>>;

struct Action
{
    std::function<int(const std::vector<Operand>&)> func;
    std::vector<Operand> operands;

    int execute() const
    {
        return func(operands);
    }

    bool isAllOperandFilled() const
    {
        for (const auto& operand : operands) {
            if (!operand.second.has_value()) {
                return false;
            }
        }
        return true;
    }
};

//////////////////////
// Action functions
//////////////////////

int printAns(const std::vector<Operand>& operands);
int assign(const std::vector<Operand>& operands);
int opposite(const std::vector<Operand>& operands);
int add(const std::vector<Operand>& operands);
int sub(const std::vector<Operand>& operands);
int mul(const std::vector<Operand>& operands);
int div(const std::vector<Operand>& operands);

}  // namespace PL0