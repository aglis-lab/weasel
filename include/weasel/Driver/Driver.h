#pragma once

#include <llvm/IR/LLVMContext.h>

#include "weasel/Parser/Parser.h"
#include "weasel/AST/AST.h"
#include "weasel/IR/Codegen.h"

namespace weasel
{
    class Driver
    {
    private:
        Codegen *_codegen;
        Module *_module;
        std::string _err;

    public:
        Driver(Codegen *codegen, Module *module);

        bool compile(std::string targetTriple = "");

        void createObject(std::string outputFile) const;
        void createIR(std::string outputFile) const;

        vector<FunctionHandle> &getFunctions() { return _module->getFunctions(); }
        vector<StructTypeHandle> &getUserTypes() { return _module->getUserTypes(); }
        FunctionHandle findFunction(string_view funName) { return _module->findFunction(funName); }

        llvm::Module *getModule() const { return _codegen->getModule(); }
        llvm::LLVMContext *getContext() const { return _codegen->getContext(); }

        std::string getError() const { return _err; }
    };

} // namespace weasel
