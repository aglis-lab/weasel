//
// Created by zaen on 27/06/21.
//
#include <iostream>
#include <fstream>
#include <llvm/IR/Function.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>

#include "weasel/Symbol/Symbol.h"
#include "weasel/Codegen/Codegen.h"
#include "weasel/Passes/Passes.h"
#include "weasel/Metadata/Metadata.h"

weasel::Codegen::Codegen(Context *context, weasel::Parser *parser)
{
    _context = context;
    _parser = parser;
}

bool weasel::Codegen::compile()
{
    auto pass = Passes(getModule());
    for (const auto &item : getFunctions())
    {
        auto identifier = item->getIdentifier();
        auto exist = getModule()->getFunction(identifier);
        if (exist)
        {
            _err = "Function conflict : " + identifier + "\n";
            return false;
        }

        auto obj = item->codegen(_context);
        assert(obj != nullptr);

        auto fun = llvm::dyn_cast<llvm::Function>(obj);
        assert(fun != nullptr);

        if (llvm::verifyFunction(*fun, &llvm::errs()))
        {
            _err = "Error when verifying function " + identifier + "\n";
            return false;
        }

        pass.run(*fun);
    }

    auto cpu = "generic";
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    auto features = "";
    auto dataLayout = llvm::DataLayout(llvm::StringRef());
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, _err);
    assert(target != nullptr);

    auto targetOpts = llvm::TargetOptions();
    auto rm = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, targetOpts, rm);

    dataLayout = targetMachine->createDataLayout();

    getModule()->setTargetTriple(targetTriple);
    getModule()->setDataLayout(dataLayout);

    Metadata(getContext()).initModule(getModule());
    if (llvm::verifyModule(*getModule()))
    {
        _err = "Error when constructing module\n";

        llvm::errs() << *getModule();

        return false;
    }

    return true;
}

void weasel::Codegen::createIR(char *outputFile) const
{
    std::error_code errCode;
    llvm::raw_fd_ostream dest(std::string(outputFile) + ".ir", errCode, llvm::sys::fs::OF_None);
    if (errCode)
    {
        llvm::errs() << "Could not open file : " << errCode.message() << "\n";
        exit(1);
    }

    dest << *getModule();
    dest.flush();
}

void weasel::Codegen::createObject(char *outputFile) const
{
    std::string err;
    std::error_code errCode;
    llvm::raw_fd_ostream dest(outputFile, errCode, llvm::sys::fs::OF_None);
    if (errCode)
    {
        llvm::errs() << "Could not open file : " << errCode.message() << "\n";
        return;
    }

    auto pass = llvm::legacy::PassManager();
    auto fileType = llvm::CodeGenFileType::CGFT_ObjectFile;
    auto targetTriple = getModule()->getTargetTriple();
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, err);
    if (target == nullptr)
    {
        llvm::errs() << err;
        exit(1);
    }

    auto cpu = "generic";
    auto features = "";
    auto rm = llvm::Optional<llvm::Reloc::Model>();
    auto targetOpts = llvm::TargetOptions();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, targetOpts, rm);

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType))
    {
        llvm::errs() << "The Target Machine can't emit a file of this type";
        exit(1);
    }

    if (!pass.run(*getModule()))
    {
        llvm::errs() << "Pass Manager failed\n";
        llvm::errs() << *getModule();
        exit(1);
    }

    dest.flush();
}
