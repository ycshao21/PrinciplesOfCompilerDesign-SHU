#pragma once
#include <vector>

namespace PL0
{

namespace Action
{
//////////////////////
// 动作函数
//////////////////////

int print(const std::vector<int>& operands);
int assign(const std::vector<int>& operands);
int opposite(const std::vector<int>& operands);
int add(const std::vector<int>& operands);
int sub(const std::vector<int>& operands);
int mul(const std::vector<int>& operands);
int div(const std::vector<int>& operands);
}  // namespace Action

}  // namespace PL0