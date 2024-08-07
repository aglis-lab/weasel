#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <llvm/Support/TargetSelect.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/LegacyPassManager.h>

#include <LLVMSPIRVLib.h>

#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/AST/AST.h"
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

    // Initialize LLVM TO BULK
    std::cout << "Initializing...\n";
    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

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

    std::cout << "Compile...\n";
    auto isCompileSuccess = codegen.compile("spir64-unknown-unknown");
    if (!isCompileSuccess)
    {
        if (!codegen.getError().empty())
        {
            std::cerr << "Codegen Compile : " << codegen.getError() << "\n";
        }
    }

    std::string errMsg;
    std::stringbuf outstreamBuf;
    std::ostream outstream(&outstreamBuf);
    bool Success = llvm::writeSpirv(codegen.getModule(), outstream, errMsg);
    if (!Success)
    {
        llvm::errs() << "Failed to translate SPIR-V: " << errMsg << '\n';
        return -1;
    }

    std::ofstream outFile(std::string(outputPath) + ".spv");
    outFile << outstreamBuf.str();
}
