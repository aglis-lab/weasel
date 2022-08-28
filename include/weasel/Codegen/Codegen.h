#pragma once

#include <llvm/IR/LLVMContext.h>

#include "weasel/Parser/Parser.h"
#include "weasel/AST/AST.h"
#include "weasel/IR/Context.h"

namespace weasel
{
    class Codegen
    {
    private:
        Context *_context;
        Parser *_parser;
        std::string _err;

    public:
        Codegen(Context *context, Parser *parser);

        bool compile(std::string targetTriple = "");

        void createObject(char *outputFile) const;
        void createIR(char *outputFile) const;

        inline std::list<Function *> getFunctions() const { return _parser->getFunctions(); }
        inline std::list<StructType *> getUserTypes() const { return _parser->getUserTypes(); }

    public:
        llvm::Module *getModule() const { return _context->getModule(); }
        llvm::LLVMContext *getContext() const { return _context->getContext(); }

        std::string getError() const { return _err; }
    };

} // namespace weasel
