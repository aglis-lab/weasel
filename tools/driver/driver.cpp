#include <iostream>
#include <iomanip>
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

void debug(std::vector<weasel::Function *> funs)
{
    std::cout << std::endl
              << std::setfill('=') << std::setw(40) << "=" << std::endl
              << std::endl;
    for (auto &fun : funs)
    {
        fun->debug(0);
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
    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    // Prepare Lexer and Parser
    auto lexer = new weasel::Lexer(fileManager);
    auto parser = new weasel::Parser(lexer);

    // Parse into AST
    auto funs = parser->parse();

    // TODO: Debugging
    debug(funs);

    // Prepare for codegen
    auto llvmContext = new llvm::LLVMContext();
    auto context = new weasel::Context(llvmContext, "codeModule");
    auto codegen = new weasel::Codegen(context, funs);

    weasel::SymbolTable::reset();
    if (!codegen->compile())
    {
        if (!codegen->getError().empty())
        {
            std::cerr << "Codegen Compile : " << codegen->getError() << "\n";
        }

        exit(1);
    }

    // Compile to Object
    codegen->createObject(outputPath);

    // Print out module
    std::error_code errCode;
    llvm::raw_fd_ostream dest(std::string(outputPath) + ".ir", errCode, llvm::sys::fs::OF_None);
    if (errCode)
    {
        llvm::errs() << "Could not open file : " << errCode.message() << "\n";
        exit(1);
    }

    dest << *codegen->getModule();
    dest.flush();
}
