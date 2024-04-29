#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "PL0.hpp"

static std::string encode(const PL0::Token& token)
{
    switch (token.type) {
    case PL0::TokenType::Keyword: {
        return token.value + "sym";
    }
    case PL0::TokenType::Identifier: {
        return "ident";
    }
    case PL0::TokenType::Number: {
        return "number";
    }
    case PL0::TokenType::Operator: {
        if (token.value == "+") {
            return "plus";
        } else if (token.value == "-") {
            return "minus";
        } else if (token.value == "*") {
            return "times";
        } else if (token.value == "/") {
            return "slash";
        } else if (token.value == "=") {
            return "eql";
        } else if (token.value == "#") {
            return "neq";
        } else if (token.value == "<") {
            return "lss";
        } else if (token.value == "<=") {
            return "leq";
        } else if (token.value == ">") {
            return "gtr";
        } else if (token.value == ">=") {
            return "geq";
        } else if (token.value == ":=") {
            return "becomes";
        } else {
            throw std::runtime_error(std::format("Unknown operator: {}", token.value));
        }
    }
    case PL0::TokenType::Delimiter: {
        if (token.value == "(") {
            return "lparen";
        } else if (token.value == ")") {
            return "rparen";
        } else if (token.value == ",") {
            return "comma";
        } else if (token.value == ";") {
            return "semicolon";
        } else if (token.value == ".") {
            return "period";
        } else {
            throw std::runtime_error(std::format("Unknown delimiter: {}", token.value));
        }
    }
    }
    return "nul";
}

void analyzeLexical(const std::string& srcFile, const std::string& outputFile)
{
    PL0::Lexer lexer(srcFile);

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: {}", outputFile));
    }
    for (PL0::Token token = lexer.getNextToken(); token.type != PL0::TokenType::EndOfFile;
         token = lexer.getNextToken()) {
        std::string encodedType = encode(token);
        output << std::format("({}, {})", encodedType, token.value) << std::endl;
    }
    output.close();
}

int main(int argc, char* argv[])
{
    PL0::ArgParser argParser;
    argParser.addOption("f", "The source file to be compiled", "string");
    argParser.addOption("o", "The output file", "string", "a.out");
    argParser.parse(argc, argv);

    std::string srcFile = *(argParser.get<std::string>("f"));
    std::cout << "Source file: " << srcFile << std::endl;

    std::string outputFile = *(argParser.get<std::string>("o"));
    std::cout << "Output file: " << outputFile << std::endl;

    analyzeLexical(srcFile, outputFile);
}