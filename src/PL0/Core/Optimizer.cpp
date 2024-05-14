#include "PL0/Core/Optimizer.hpp"
#include "PL0/Utils/Str2Num.hpp"

#include <algorithm>
#include <format>
#include <functional>
#include <iostream>

namespace PL0
{
std::vector<Quadruple> Optimizer::optimize(const std::vector<Quadruple>& quads)
{
    generateDAG(quads);

    std::vector<Quadruple> newQuads;
    for (const NodePtr& node : m_nodes) {
        if (node->names.empty()) {
            continue;
        }

        Quadruple quad;
        // 如果结点没有操作数，那么它是一个常数或标识符，将值赋给结果的第一个变量名。
        if (node->operands.empty()) {
            quad = {"=", node->value, "", node->names[0]};
        }
        // 否则，将操作符和操作数赋给结果的第一个变量名。
        // 其中，如果操作数是常数，将其值赋给结果，否则将其名字赋给结果。
        else {
            NodePtr operand1Node = node->operands[0];
            std::string operand1 =
                operand1Node->constant.has_value() ? operand1Node->value : operand1Node->names[0];

            NodePtr operand2Node = node->operands[1];
            std::string operand2 =
                operand2Node->constant.has_value() ? operand2Node->value : operand2Node->names[0];

            quad = {node->value, operand1, operand2, node->names[0]};
        }
        newQuads.push_back(quad);

        // 如果有多个变量名，将第一个变量名赋给其他变量名。
        for (size_t nameIndex = 1; nameIndex < node->names.size(); ++nameIndex) {
            Quadruple quad{"=", node->names[0], "", node->names[nameIndex]};
            newQuads.push_back(quad);
        }
    }

    return newQuads;
}

void Optimizer::generateDAG(const std::vector<Quadruple>& quads)
{

    for (auto& [op, operand1, operand2, result] : quads) {
        NodePtr node1 = nullptr;
        bool node1Exists = m_nodePtrMap.contains(operand1);
        if (node1Exists) {
            node1 = m_nodePtrMap[operand1];
        } else {
            // 为 operand1 创建一个新结点。
            node1 = std::make_shared<DAGNode>();
            node1->names.push_back(operand1);
            node1->value = operand1;
            node1->constant = str2num(operand1);
        }

        NodePtr curNode = nullptr;

        /**
         * @note 一个四元式的形式为 (op, operand1, operand2, result)。
         *      一共有两种有效情况：
         *        1. (op, operand1, operand2, result)
         *        2. (= , operand1, _, result)
        */
        if (!operand2.empty()) {  // 情况1: (op, operand1, operand2, result)
            NodePtr node2 = nullptr;
            bool node2Exists = m_nodePtrMap.contains(operand2);
            if (node2Exists) {
                node2 = m_nodePtrMap[operand2];
            } else {
                // 为 operand2 创建一个新结点。
                node2 = std::make_shared<DAGNode>();
                node2->names.push_back(operand2);
                node2->value = operand2;
                node2->constant = str2num(operand2);
            }

            /**
             * @note 如果两个操作数都是常数，计算出结果（常数）。
            */
            if (node1->constant.has_value() && node2->constant.has_value()) {
                int resultVal = calculate(op, node1->constant.value(), node2->constant.value());
                std::string resultValStr = std::to_string(resultVal);

                // 如果存在值为 resultValStr 的结点，直接使用它。
                if (m_nodePtrMap.contains(resultValStr)) {
                    curNode = m_nodePtrMap[resultValStr];
                }
                // 否则，为结果创建一个新结点。
                else {
                    curNode = std::make_shared<DAGNode>();
                    curNode->value = resultValStr;
                    curNode->names = {};
                    curNode->constant = resultVal;
                    // 将结果添加到映射表中。
                    m_nodePtrMap[resultValStr] = curNode;
                }
            }
            /**
             * @note 如果有一个操作数不是常数，查找值为 op，操作数为 node1 和 node2 的结点是否存在。
             *     如果存在，使用它作为操作符结点。否则，为操作符创建一个新结点。
             * @note 如果操作数是 node2 和 node1，而不是 node1 和 node2，不可以直接使用已有的操作符结点，
             *     必须创建一个新的操作符结点。
            */
            else {
                // 如果操作数为 node1 和 node2 的结点不存在，将它们添加到映射表中。
                if (!node1Exists) {
                    m_nodePtrMap[operand1] = node1;
                }
                if (!node2Exists) {
                    m_nodePtrMap[operand2] = node2;
                }

                auto nodeIt = std::ranges::find_if(m_nodes, [&](NodePtr node) {
                    return node->value == op && node->operands.size() == 2 &&
                           node->operands[0] == m_nodePtrMap[operand1] &&
                           node->operands[1] == m_nodePtrMap[operand2];
                });
                // 如果操作符结点存在，使用它。
                if (nodeIt != m_nodes.end()) {
                    curNode = *nodeIt;
                }
                // 否则，为操作符创建一个新结点。
                else {
                    curNode = std::make_shared<DAGNode>();
                    curNode->names = {};
                    curNode->value = op;
                    curNode->operands = {node1, node2};
                    curNode->constant = std::nullopt;
                }
            }
        } else if (op == "=") {  // 情况2: (= , operand1, _, result)
            if (!node1Exists) {
                /**
                 * @note 如果 operand1 的结点不存在，它是一个常数或标识符，e.g. T1 := 3, T2 := a。
                 *      需要移除设置的变量名，否则会看到 (=, 3, _, 3) 和 (=, a, _, a)这样的四元组
                */
                node1->names.pop_back();
                m_nodePtrMap[operand1] = node1;
            }
            // 如果结果的结点存在，使用它。
            curNode = node1;
        } else {
            throw std::runtime_error("Unknown operation.");
        }

        // 将结果的名字添加到操作符结点。
        curNode->names.push_back(result);

        /**
         * @note 如果结果已经在映射表中，说明它的值现在被更新了，需要将它从原来的结点中移除。
        */
        auto resultIt = m_nodePtrMap.find(result);
        if (resultIt != m_nodePtrMap.end()) {
            auto [_, resultNode] = *resultIt;
            auto resultNameIt = std::ranges::find(resultNode->names, result);
            resultNode->names.erase(resultNameIt);
        }

        // 将结果重新映射到操作符结点。
        m_nodePtrMap[result] = curNode;

        auto nodeIt = std::ranges::find(m_nodes, curNode);
        if (nodeIt == m_nodes.end()) {
            m_nodes.push_back(curNode);
        }
    }
}

}  // namespace PL0