#include <iostream>
#include <sstream>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/LegacyPassManager.h>
#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/AST/AST.h"
#include "weasel/Symbol/Symbol.h"
#include "weasel/Basic/FileManager.h"
#include "weasel/Codegen/Codegen.h"

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
    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    // Prepare Lexer and Parser
    auto lexer = new weasel::Lexer(fileManager);
    auto parser = new weasel::Parser(lexer);

    // Parse into AST
    auto funs = parser->parse();

    // Prepare for codegen
    auto llvmContext = new llvm::LLVMContext();
    auto context = new weasel::Context(llvmContext, "codeModule");
    auto codegen = new weasel::Codegen(context, funs);

    std::cout << "Try to compile\n";

    weasel::SymbolTable::reset();
    if (!codegen->compile())
    {
        std::cerr << "Codegen Compile : " << codegen->getError() << "\n";
        exit(1);
    }

    llvm::errs() << llvmContext;

    // Compile to Object
    codegen->createObject(outputPath);
}
