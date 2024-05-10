#pragma once
#include <exception>
#include <string>

namespace PL0
{
class Error : public std::exception
{
public:
    Error(const std::string& msg) : m_msg(msg)
    {
    }

    virtual const char* what() const noexcept override
    {
        return m_msg.c_str();
    }
protected:
    std::string m_msg;
};

class LexicalError : public Error
{
public:
    LexicalError(const std::string& msg) : Error(msg)
    {
    }
};

class SyntaxError : public Error
{
public:
    SyntaxError(const std::string& msg) : Error(msg)
    {
    }
};

class SemanticError : public Error
{
public:
    SemanticError(const std::string& msg) : Error(msg)
    {
    }
};
}  // namespace PL0
