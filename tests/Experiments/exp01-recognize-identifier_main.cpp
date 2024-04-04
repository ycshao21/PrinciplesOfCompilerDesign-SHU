#include <algorithm>
#include <format>
#include <fstream>
// #include <iostream>
#include <string>
#include <vector>

#include "PL0.hpp"

struct IdentInfo
{
    std::string ident = "";
    int count = 0;
};

void recognizeIdent(const std::string& srcFile, const std::string& outputFile)
{
    PL0::Scanner scanner(srcFile);
    PL0::Lexer lexer(scanner);

    std::vector<IdentInfo> identifiers;

    for (PL0::Word word = lexer.getNextWord(); word.type != PL0::WordType::EndOfFile;
         word = lexer.getNextWord()) {
        if (word.type == PL0::WordType::Identifier) {
            auto fn = [&word](const IdentInfo& info) { return info.ident == word.value; };
            auto it = std::ranges::find_if(identifiers, fn);

            if (it == identifiers.end()) {
                identifiers.push_back({word.value, 1});
            } else {
                it->count++;
            }
        }
    }

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to open file: " + outputFile);
    }
    for (const auto& [ident, count] : identifiers) {
        output << std::format("({}: {})", ident, count) << std::endl;
    }
    output.close();
}

int main(int argc, char* argv[])
{
    PL0::ArgParser argParser;
    argParser.addOption("f", "The source file to be compiled", "string");
    argParser.addOption("o", "The output file", "string", "a.out");
    argParser.parse(argc, argv);

    std::string srcFile, outputFile;
    auto value = argParser.get<std::string>("f");
    std::cout << "Source file: " << *value << std::endl;
    srcFile = *value;

    value = argParser.get<std::string>("o");
    std::cout << "Output file: " << *value << std::endl;
    outputFile = *value;

    recognizeIdent(srcFile, outputFile);
}