#pragma once
#include <exception>
#include <string>
#include <format>

namespace PL0
{
class Error : public std::exception
{
public:
    virtual const char* what() const noexcept override
    {
        return m_msg.c_str();
    }
protected:
    std::string m_msg;
};

class SyntaxError : public Error
{
public:
    SyntaxError(const std::string& msg)
    {
        m_msg = std::format("Syntax error: {}", msg);
    }
};

class SemanticError : public Error
{
public:
    SemanticError(const std::string& msg)
    {
        m_msg = std::format("Semantic error: {}", msg);
    }
};
}  // namespace PL0
