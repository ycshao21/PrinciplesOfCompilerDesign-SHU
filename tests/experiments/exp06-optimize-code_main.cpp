#include "PL0.hpp"

void optimizeCode(const std::string& srcFile, const std::string& outputFile)
{
    std::ifstream input(srcFile);
    if (!input.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: {}", srcFile));
    }

    std::vector<PL0::Quadruple> quads;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<std::string> quadParts;
        std::string part;
        std::istringstream lineStream(line);
        while (std::getline(lineStream, part, ',')) {
            quadParts.push_back(part);
        }

        PL0::Quadruple quad;
        quad.op = quadParts[0];
        quad.operand1 = quadParts[1];
        quad.operand2 = quadParts[2];
        quad.result = quadParts[3];
        quads.push_back(quad);
    }
    input.close();

    PL0::Optimizer optimizer;
    std::vector<PL0::Quadruple> optimizedQuads = optimizer.optimize(quads);

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: {}", outputFile));
    }
    for (const PL0::Quadruple& quad : optimizedQuads) {
        output << std::format("{},{},{},{}", quad.op, quad.operand1, quad.operand2,
                              quad.result)
               << std::endl;
    }
    output.close();
}

int main(int argc, char* argv[])
{
    PL0::ArgParser argParser;
    argParser.addOption("f", "The source file to be compiled", "string");
    argParser.addOption("o", "The output file", "string");
    argParser.parse(argc, argv);

    std::string srcFile = *(argParser.get<std::string>("f"));
    std::cout << "Source file: " << srcFile << std::endl;

    std::string outputFile = *(argParser.get<std::string>("o"));
    std::cout << "Output file: " << outputFile << std::endl;

    optimizeCode(srcFile, outputFile);
}