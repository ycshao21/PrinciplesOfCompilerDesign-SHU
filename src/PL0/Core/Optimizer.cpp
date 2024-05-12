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
        // If the node has no operands, it is a constant or an identifier.
        // Assign the value to the FIRST NAME of the result.
        if (node->operands.empty()) {
            quad = {"=", node->value, "", node->names[0]};
        }
        // Otherwise, assign the operator and operands to the first name of the result.
        // If an operand is a constant, assign its value, otherwise assign its name.
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

        // If there are multiple names for the result, assign the first name to other names.
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
            // Create a new node for operand1.
            node1 = std::make_shared<DAGNode>();
            node1->names.push_back(operand1);
            node1->value = operand1;
            node1->constant = str2num(operand1);
        }

        NodePtr curNode = nullptr;

        /**
         * @note A quadruple is in the form of (op, operand1, operand2, result).
         *      There are only two valid cases:
         *     - Case1: (op, operand1, operand2, result)
         *     - Case2: (= , operand1, _, result)
         */
        if (!operand2.empty()) {  // Case1: (op, operand1, operand2, result)
            NodePtr node2 = nullptr;
            bool node2Exists = m_nodePtrMap.contains(operand2);
            if (node2Exists) {
                node2 = m_nodePtrMap[operand2];
            } else {
                // Create a new node for operand2.
                node2 = std::make_shared<DAGNode>();
                node2->names.push_back(operand2);
                node2->value = operand2;
                node2->constant = str2num(operand2);
            }

            /**
             * @note If both operands are constants, calculate the result (which is also a constant)
             * and create a new node for the result (if no such node exists).
             */
            if (node1->constant.has_value() && node2->constant.has_value()) {
                int resultVal = calculate(op, node1->constant.value(), node2->constant.value());
                std::string resultValStr = std::to_string(resultVal);

                // If a node whose value is resultValStr exists, use it directly.
                if (m_nodePtrMap.contains(resultValStr)) {
                    curNode = m_nodePtrMap[resultValStr];
                }
                // If no such node exists, create a new node for the result.
                else {
                    curNode = std::make_shared<DAGNode>();
                    curNode->value = resultValStr;
                    curNode->names = {};
                    curNode->constant = resultVal;
                    // Add the node to the map.
                    m_nodePtrMap[resultValStr] = curNode;
                }
            }
            /**
             * @note If there is a non-constant operand, find if a node whose value is op and
             *      operands are node1 and node2 exists. If it exists, use it as the operator node.
             *      Otherwise, create a new node for the operator.
             * @note If the operands are node2 and node1, not node1 and node2, we still need to
             *      create a new node for the operator.
             */
            else {
                // If the nodes of operand1 and operand2 do not exist, add them to the map.
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
                // If the operator node exists, use it.
                if (nodeIt != m_nodes.end()) {
                    curNode = *nodeIt;
                }
                // If not, create a new node for the operator.
                else {
                    curNode = std::make_shared<DAGNode>();
                    curNode->names = {};
                    curNode->value = op;
                    curNode->operands = {node1, node2};
                    curNode->constant = std::nullopt;
                }
            }
        } else if (op == "=") {  // Case2: (= , operand1, _, result)
            if (!node1Exists) {
                /**
                 * @note If the node of operand1 does not exist, it is a constant or an identifier.
                 *      e.g. T1 := 3, T2 := a.
                 *      We need to pop the name, otherwise, we will see (=, 3, _, 3) and (=, a, _,
                 *      a).
                 */
                node1->names.pop_back();
                m_nodePtrMap[operand1] = node1;
            }
            // Use the node of operand1 as the operator node if it exists.
            curNode = node1;
        } else {
            throw std::runtime_error("Unknown operation.");
        }

        // Add the name of result to the operator node.
        curNode->names.push_back(result);

        /**
         * @note If the result is already in the map, it means that its value has been updated.
         */
        auto resultIt = m_nodePtrMap.find(result);
        if (resultIt != m_nodePtrMap.end()) {
            auto [_, resultNode] = *resultIt;
            auto resultNameIt = std::ranges::find(resultNode->names, result);
            resultNode->names.erase(resultNameIt);
        }

        // Remap the result to the operator node.
        m_nodePtrMap[result] = curNode;

        auto nodeIt = std::ranges::find(m_nodes, curNode);
        if (nodeIt == m_nodes.end()) {
            m_nodes.push_back(curNode);
        }
    }
}

}  // namespace PL0