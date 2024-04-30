#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "PL0.hpp"

void analyzeSyntax(const std::string& srcFile)
{
    // PL0::Lexer lexer;
    // std::vector<PL0::Token> tokens = lexer.tokenize(srcFile);

    // PL0::Parser parser;
    // bool success = parser.parse(tokens);
    // if (success) {
    //     PL0::Reporter::info("Parsing succeeded!");
    // } else {
    //     PL0::Reporter::error("Parsing failed!");
    // }
}

int main(int argc, char* argv[])
{
    PL0::ArgParser argParser;
    argParser.addOption("f", "The source file to be compiled", "string");
    argParser.parse(argc, argv);

    std::string srcFile = *(argParser.get<std::string>("f"));
    std::cout << "Source file: " << srcFile << std::endl;

    analyzeSyntax(srcFile);
}