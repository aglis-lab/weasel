#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/AST/AST.h"
#include "weasel/Symbol/Symbol.h"
#include "weasel/Basic/FileManager.h"
#include "weasel/Codegen/Codegen.h"
#include "weasel/Analysis/AnalysisSemantic.h"
#include "weasel/Passes/PassesGLSL.h"

#include <glog/logging.h>

void debug(const std::vector<weasel::Function *> &objects)
{
    std::cout << std::endl
              << std::setfill('=') << std::setw(40) << "=" << std::endl
              << std::endl;

    for (auto &obj : objects)
    {
        obj->debug(0);
    }

    std::cout << std::endl
              << std::setfill('=') << std::setw(40) << "=" << std::endl
              << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        std::cerr << "Not Input files\n";
        return 1;
    }

    auto filePath = argv[1];
    // auto outputPath = argv[2];
    auto fileManager = weasel::FileManager(filePath);
    if (!fileManager.isValid())
    {
        std::cout << filePath << " Not exist\n";
        return 0;
    }

    // Prepare Lexer and Parser
    auto lexer = weasel::Lexer(&fileManager);
    auto parser = weasel::Parser(&lexer);

    // Parse into AST
    LOG(INFO) << "Parsing...\n";
    parser.parse();

    // Debugging AST
    LOG(INFO) << "Debug AST...\n";
    debug(parser.getFunctionsParallel());

    // Transpiling to GLSL Shading Language
    LOG(INFO) << "Transpiling AST to GLSL ...\n";
    auto pass = weasel::PassesGLSL(parser.getFunctionsParallel());

    pass.run();
}
