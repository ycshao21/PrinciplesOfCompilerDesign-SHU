#pragma once
#include "Token.hpp"
#include <vector>
#include <memory>

namespace PL0
{
class Parser
{
public:
    Parser() = default;
    virtual ~Parser() = default;

    virtual void parse(const std::vector<Token>& tokens) = 0;
};

}  // namespace PL0