#pragma once
#include "PL0/Utils/Exception.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace PL0
{
struct Quadruple
{
    std::string op;
    std::string operand1;
    std::string operand2;
    std::string result;
};

/**
 * @note 操作符结点
 *      e.g. T1 := a + b, T2 := a + b
 *          - names: {T1, T2}
 *          - value: "+"
 *          - operands: {a, b}
 *          - constValue: nullopt
 *
 *       常数结点
 *          a) T1 := 3
 *              - names: {T1}
 *              - value: "3"
 *              - operands: (空)
 *              - constValue: 3
 *          b) T1 := 2 + a 中的 2
 *              - names: (空)
 *              - value: "2"
 *              - operands: (空)
 *              - constValue: 2
 *
 *       标识符结点
 *          a) T1 := a
 *              - names: {T1}
 *              - value: "a"
 *              - operands: (空)
 *              - constValue: nullopt
 *          b) T1 := 2 + a 中的 a
 *              - names: (空)
 *              - value: "a"
 *              - operands: (空)
 *              - constValue: nullopt
 *
 */
struct DAGNode
{
    using NodePtr = std::shared_ptr<DAGNode>;

    std::vector<std::string> names;  // 结点所表示的变量名
    std::string value;  // 结点的值，可为操作符、常数、标识符
    std::vector<NodePtr> operands;  // 操作数 （如果是操作符结点）
    std::optional<int> constant;    // 常数值（如果是常数结点）
};

class Optimizer
{
    using NodePtr = std::shared_ptr<DAGNode>;

public:
    Optimizer() = default;

    std::vector<Quadruple> optimize(const std::vector<Quadruple>& quads);

private:
    void generateDAG(const std::vector<Quadruple>& quads);

    int calculate(const std::string& op, int val1, int val2)
    {
        if (op == "+") {
            return val1 + val2;
        } else if (op == "-") {
            return val1 - val2;
        } else if (op == "*") {
            return val1 * val2;
        } else if (op == "/") {
            if (val2 == 0) {
                throw SemanticError("Division by zero.");
            }
            return val1 / val2;
        } else {
            throw std::runtime_error("Invalid operator for calculation.");
        }
    }

private:
    std::vector<NodePtr> m_nodes;  // DAG 结点
    std::map<std::string, NodePtr> m_nodePtrMap;  // 将变量名映射到结点指针，用于查找结点
};

}  // namespace PL0