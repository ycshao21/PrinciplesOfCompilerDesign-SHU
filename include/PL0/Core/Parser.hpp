#pragma once
#include "PL0/Core/Token.hpp"
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

protected:
    std::unique_ptr<std::vector<Token>> m_tokens = nullptr;
    size_t m_curIndex = 0;
};

}  // namespace PL0