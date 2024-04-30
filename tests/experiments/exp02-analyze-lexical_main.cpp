#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "PL0.hpp"

void analyzeLexical(const std::string& srcFile, const std::string& outputFile)
{
    PL0::Lexer lexer;
    std::vector<PL0::Token> tokens = lexer.tokenize(srcFile);

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: {}", outputFile));
    }
    for (const PL0::Token& token : tokens) {
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