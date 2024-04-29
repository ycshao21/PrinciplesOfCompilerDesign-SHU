#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "PL0.hpp"

static std::string encode(const PL0::Word& word)
{
    switch (word.type) {
    case PL0::WordType::Keyword: {
        return word.value + "sym";
    }
    case PL0::WordType::Identifier: {
        return "ident";
    }
    case PL0::WordType::Number: {
        return "number";
    }
    case PL0::WordType::Operator: {
        if (word.value == "+") {
            return "plus";
        } else if (word.value == "-") {
            return "minus";
        } else if (word.value == "*") {
            return "times";
        } else if (word.value == "/") {
            return "slash";
        } else if (word.value == "=") {
            return "eql";
        } else if (word.value == "#") {
            return "neq";
        } else if (word.value == "<") {
            return "lss";
        } else if (word.value == "<=") {
            return "leq";
        } else if (word.value == ">") {
            return "gtr";
        } else if (word.value == ">=") {
            return "geq";
        } else if (word.value == ":=") {
            return "becomes";
        } else {
            throw std::runtime_error(std::format("Unknown operator: {}", word.value));
        }
    }
    case PL0::WordType::Delimiter: {
        if (word.value == "(") {
            return "lparen";
        } else if (word.value == ")") {
            return "rparen";
        } else if (word.value == ",") {
            return "comma";
        } else if (word.value == ";") {
            return "semicolon";
        } else if (word.value == ".") {
            return "period";
        } else {
            throw std::runtime_error(std::format("Unknown delimiter: {}", word.value));
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
    for (PL0::Word word = lexer.getNextWord(); word.type != PL0::WordType::EndOfFile;
         word = lexer.getNextWord()) {
        std::string encodedType = encode(word);
        output << std::format("({}, {})", encodedType, word.value) << std::endl;
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