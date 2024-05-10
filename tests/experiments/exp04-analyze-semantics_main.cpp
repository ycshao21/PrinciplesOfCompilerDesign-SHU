#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "PL0.hpp"

#include <functional>

void analyzeSemantics(const std::string& srcFile)
{
    PL0::Lexer lexer;
    std::vector<PL0::Token> tokens = lexer.tokenize(srcFile);

    PL0::SemanticLL1Parser parser;
    parser.parse(tokens);
}

int main(int argc, char* argv[])
{
    PL0::ArgParser argParser;
    argParser.addOption("f", "The source file to be compiled", "string");
    argParser.parse(argc, argv);

    std::string srcFile = *(argParser.get<std::string>("f"));
    std::cout << "Source file: " << srcFile << std::endl;

    analyzeSemantics(srcFile);
}