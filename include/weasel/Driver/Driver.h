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
        WeaselCodegen *_codegen;
        Parser *_parser;
        std::string _err;

    public:
        Driver(WeaselCodegen *codegen, Parser *parser);

        bool compile(std::string targetTriple = "");

        void createObject(char *outputFile) const;
        void createIR(char *outputFile) const;

        inline std::vector<Function *> getFunctions() const { return _parser->getFunctions(); }
        inline std::vector<StructType *> getUserTypes() const { return _parser->getUserTypes(); }

    public:
        llvm::Module *getModule() const { return _codegen->getModule(); }
        llvm::LLVMContext *getContext() const { return _codegen->getContext(); }

        std::string getError() const { return _err; }
    };

} // namespace weasel
