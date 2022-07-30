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

    private:
        inline std::list<Function *> getFunctions() const { return _parser->getFunctions(); }

    public:
        Codegen(Context *context, Parser *parser);

        bool compile();

        void createObject(char *outputFile) const;
        void createIR(char *outputFile) const;

    public:
        llvm::Module *getModule() const { return _context->getModule(); }
        llvm::LLVMContext *getContext() const { return _context->getContext(); }

        std::string getError() const { return _err; }
    };

} // namespace weasel
