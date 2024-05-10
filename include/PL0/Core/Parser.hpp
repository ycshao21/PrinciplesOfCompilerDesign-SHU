#pragma once
#include "Token.hpp"
#include <vector>
#include <memory>

namespace PL0
{
/**
 * @brief The Parser class is an abstract class that defines the interface for parsing tokens.
*/
class Parser
{
public:
    virtual void parse(const std::vector<Token>& tokens) = 0;
};

}  // namespace PL0