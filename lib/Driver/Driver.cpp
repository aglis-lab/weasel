//
// Created by zaen on 27/06/21.
//
#include <iostream>
#include <fstream>

#include <llvm/IR/Function.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Scalar/MemCpyOptimizer.h>
#include <llvm/IR/PassManager.h>

#include <weasel/Driver/Driver.h>
#include <weasel/Passes/Passes.h>
#include <weasel/Metadata/Metadata.h>

Driver::Driver(WeaselCodegen *codegen, Module *module)
{
    _codegen = codegen;
    _module = module;
}

bool Driver::compile(std::string defTargetTriple)
{
    LOG(INFO) << "Try Codegen...\n";

    auto pass = Passes(getModule());
    for (const auto &item : getFunctions())
    {
        // Check function conflict
        auto identifier = item->getIdentifier();
        auto exist = getModule()->getFunction(identifier);
        if (exist)
        {
            _err = "Function conflict : " + identifier + "\n";
            return false;
        }

        // Codegen Function
        auto obj = item->codegen(_codegen);
        assert(obj != nullptr);

        // Casting function obj to llvm function
        auto fun = llvm::dyn_cast<llvm::Function>(obj);
        assert(fun != nullptr);

        // Verify Function
        if (llvm::verifyFunction(*fun, &llvm::errs()))
        {
            _err = "Error when verifying function " + identifier + "\n";
            llvm::errs() << getModule();
            return false;
        }

        // if (item->getType()->isStructType())
        // {
        //     auto analysis = llvm::FunctionAnalysisManager();
        //     llvm::MemCpyOptPass().run(*fun, analysis);
        // }

        // Give Passes to the function
        pass.run(*fun);
    }

    LOG(INFO) << "Finish Codegen...\n";

    if (!defTargetTriple.empty())
    {
        getModule()->setTargetTriple(defTargetTriple);
    }
    else
    {
        auto cpu = "generic";
        auto targetTriple = llvm::sys::getDefaultTargetTriple();
        auto features = "";
        auto dataLayout = llvm::DataLayout(llvm::StringRef());
        auto target = llvm::TargetRegistry::lookupTarget(targetTriple, _err);
        assert(target != nullptr);

        auto targetOpts = llvm::TargetOptions();
        auto rm = optional<llvm::Reloc::Model>();
        auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, targetOpts, rm);

        dataLayout = targetMachine->createDataLayout();

        getModule()->setTargetTriple(targetTriple);
        getModule()->setDataLayout(dataLayout);
    }

    Metadata(getContext(), getModule()).initModule(getModule());
    if (llvm::verifyModule(*getModule()))
    {
        _err = "Error when constructing module\n";
        return false;
    }

    return true;
}

void Driver::createIR(std::string outputFile) const
{
    std::error_code errCode;
    llvm::raw_fd_ostream dest(outputFile + ".ir", errCode, llvm::sys::fs::OF_None);
    if (errCode)
    {
        llvm::errs() << "Could not open file : " << errCode.message() << "\n";
        exit(1);
    }

    dest << *getModule();
    dest.flush();
}

void Driver::createObject(std::string outputFile) const
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
    auto rm = optional<llvm::Reloc::Model>();
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
