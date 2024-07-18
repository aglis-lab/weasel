//
// Created by zaen on 27/06/21.
//
#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/MDBuilder.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>

using namespace std;

namespace weasel
{
    class Metadata
    {
    private:
        llvm::LLVMContext *_context;
        llvm::MDBuilder *_MDbuilder;
        llvm::DIBuilder *_DIBuilder;

    private:
        llvm::Metadata *getFlagsMetadata();
        llvm::Metadata *getVersionMetadata();
        llvm::MDNode *getCLVersionMetadata();

        llvm::LLVMContext *getContext() const { return _context; }
        llvm::MDBuilder *getMDBuilder() { return _MDbuilder; }
        llvm::DIBuilder *getDIBuilder() { return _DIBuilder; }

    public:
        Metadata() = default;

        Metadata(llvm::LLVMContext *context, llvm::Module *module)
        {
            _context = context;
            _MDbuilder = new llvm::MDBuilder(*context);
            _DIBuilder = new llvm::DIBuilder(*module);
        }

        // void initParallelModule(llvm::Module *module) {
        //     initModule(module);

        //     module->getOrInsertNamedMetadata("opencl.ocl.version")->addOperand(getCLVersionMetadata());
        //     module->getOrInsertNamedMetadata("opencl.spir.version")->addOperand(getCLVersionMetadata());
        // }

        // Create Dbg Declare
        llvm::IntrinsicInst *createDbgDeclare(llvm::IRBuilder<> *, llvm::Module *, llvm::Value *, llvm::Metadata *, llvm::Metadata *);

        void initModule(llvm::Module *module)
        {
            module->addModuleFlag(llvm::Module::ModFlagBehavior::ModFlagBehaviorFirstVal, "wchar_size", getFlagsMetadata());
            module->getOrInsertNamedMetadata("llvm.ident")->addOperand(llvm::MDNode::get(*getContext(), {getVersionMetadata()}));
        }
    };
}
