#pragma once

#include <vector>
#include <memory>
#include "llvm/IR/LLVMContext.h"
#include "weasel/ast/ast.h"
#include "weasel/ir/context.h"
#include "weasel/config/config.h"

namespace weasel
{
    class Codegen
    {
    private:
        Context *_context;
        std::vector<Function *> _funs;
        std::string _err;

        // bool _isParallel;

    public:
        Codegen(Context *context, std::vector<Function *> funs);

        bool compile();

        // std::string createSpirv();
        void createObject(char *outputFile) const;

    public:
        llvm::Module *getModule() const { return _context->getModule(); }
        llvm::LLVMContext *getContext() const { return _context->getContext(); }

        std::string getError() const { return _err; }
    };

} // namespace weasel
