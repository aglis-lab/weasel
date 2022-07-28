#pragma once

#include <vector>
#include <memory>
#include "llvm/IR/LLVMContext.h"
#include "weasel/AST/AST.h"
#include "weasel/IR/Context.h"

namespace weasel
{
    class Codegen
    {
    private:
        Context *_context;
        std::vector<GlobalObject *> _objects;
        std::string _err;

    public:
        Codegen(Context *context, const std::vector<GlobalObject *> &objects);

        bool compile();

        void createObject(char *outputFile) const;
        void createIR(char *outputFile) const;

    public:
        llvm::Module *getModule() const { return _context->getModule(); }
        llvm::LLVMContext *getContext() const { return _context->getContext(); }

        std::string getError() const { return _err; }
    };

} // namespace weasel
