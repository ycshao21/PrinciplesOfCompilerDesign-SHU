#pragma once
#include "PL0/Utils/Error.hpp"
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
 * @note For an operator node,
 *      e.g. T1 := a + b, T2 := a + b
 *          - names: {T1, T2}
 *          - value: "+"
 *          - operands: {a, b}
 *          - constValue: nullopt
 *
 *       For a constant node,
 *          a) T1 := 3
 *              - names: {T1}
 *              - value: "3"
 *              - operands: (empty)
 *              - constValue: 3
 *          b) 2 in T1 := 2 + a
 *              - names: (empty)
 *              - value: "2"
 *              - operands: (empty)
 *              - constValue: 2
 *
 *       For an identifier node,
 *          a) T1 := a
 *              - names: {T1}
 *              - value: "a"
 *              - operands: (empty)
 *              - constValue: nullopt
 *          b) a in T1 := 2 + a
 *              - names: (empty)
 *              - value: "a"
 *              - operands: (empty)
 *              - constValue: nullopt
 *
 */
struct DAGNode
{
    using NodePtr = std::shared_ptr<DAGNode>;

    std::vector<std::string> names;  // Names of variables that this node represents.
    std::string value;  // The value of this node, can be a constant, an operator, or an identifier.
    std::vector<NodePtr> operands;  // Operands of this node (if value is an operator)
    std::optional<int> constant;    // The constant value of this node (if it is a constant).
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
    std::vector<NodePtr> m_nodes;  // A vector of all nodes in the DAG.

    std::map<std::string, NodePtr>
        m_nodePtrMap;  // A hash map mapping variable names to their corresponding nodes.
};

}  // namespace PL0