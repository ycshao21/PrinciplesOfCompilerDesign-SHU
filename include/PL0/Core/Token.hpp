#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <string>

namespace PL0
{
/////////////////////////////////////////////////////////////////////////////////////////////////
// Token
/////////////////////////////////////////////////////////////////////////////////////////////////

enum class TokenType : uint8_t
{
    Invalid = 0,
    Keyword,
    Identifier,
    Number,
    Operator,
    Delimiter
};

struct Token
{
    TokenType type;
    std::string value;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////////////////////////////////////

constexpr std::array<std::string, 13> KEYWORDS = {"const", "var",   "procedure", "begin", "end",
                                                  "if",    "then",  "while",     "do",    "call",
                                                  "odd",   "write", "read"};

constexpr std::array<char, 9> OPERATORS_CH = {'+', '-', '*', '/', '=', '#', '>', '<', ':'};

constexpr std::array<char, 5> DELIMITERS = {'(', ')', ',', ';', '.'};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////////////////////////

inline bool isDigit(char c)
{
    return std::isdigit(c);
}

inline bool isAlpha(char c)
{
    return std::isalpha(c);
}

inline bool isAlphaOrDigit(char c)
{
    return std::isalnum(c);
}

inline bool isDelimiter(char c)
{
    return std::ranges::find(DELIMITERS, c) != DELIMITERS.end();
}

inline bool isOperatorChar(char c)
{
    return std::ranges::find(OPERATORS_CH, c) != OPERATORS_CH.end();
}
}  // namespace PL0
