#include <algorithm>
#include <format>
#include <fstream>
#include <string>
#include <vector>

#include "PL0.hpp"

struct IdentInfo
{
    std::string ident = "";
    uint64_t count = 0;
};

void recognizeIdent(const std::string& srcFile, const std::string& outputFile)
{
    PL0::Lexer lexer;
    std::vector<PL0::Token> tokens = lexer.tokenize(srcFile);

    std::vector<IdentInfo> identifiers;
    for (const PL0::Token& token : tokens) {
        if (token.type == PL0::TokenType::Identifier) {
            auto fn = [&token](const IdentInfo& info) { return info.ident == token.value; };
            auto it = std::ranges::find_if(identifiers, fn);
            if (it == identifiers.end()) {
                identifiers.push_back({token.value, 1});
            } else {
                it->count++;
            }
        }
    }

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: {}", outputFile));
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

    std::string srcFile = *(argParser.get<std::string>("f"));
    std::cout << "Source file: " << srcFile << std::endl;

    std::string outputFile = *(argParser.get<std::string>("o"));
    std::cout << "Output file: " << outputFile << std::endl;

    recognizeIdent(srcFile, outputFile);
}