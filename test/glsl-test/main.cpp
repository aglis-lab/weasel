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

void debug(const std::list<weasel::Function *> &objects)
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
    if (argc <= 2)
    {
        std::cerr << "Not Input files\n";
        return 1;
    }

    auto filePath = argv[1];
    auto outputPath = argv[2];
    auto fileManager = new weasel::FileManager(filePath);
    if (!fileManager->isValid())
    {
        std::cout << filePath << " Not exist\n";
        return 0;
    }

    // Prepare Lexer and Parser
    auto lexer = weasel::Lexer(fileManager);
    auto parser = weasel::Parser(&lexer);

    // Parse into AST
    std::cout << "Parsing...\n";
    parser.parse();

    // Debugging AST
    std::cout << "Debug AST...\n";
    debug(parser.getFunctions());

    // Prepare for codegen
    auto llvmContext = llvm::LLVMContext();
    auto context = weasel::Context(&llvmContext, "codeModule");
    auto codegen = weasel::Codegen(&context, &parser);

    // TODO: Convert from Generic AST to AST HLSL
    std::cout << "Convert to HLSL...\n";
    // auto funs = codegen.getFunctions();
    // auto types = codegen.getUserTypes();

    // TODO: Transpiling to HLSL Shading Language
    std::cout << "Transpiling to HLSL\n";
}
