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

namespace weasel
{
    class Metadata
    {
    private:
        llvm::MDBuilder *_MDbuilder;
        llvm::LLVMContext *_context;
        llvm::DIBuilder *_DIBuilder;

    private:
        llvm::Metadata *getFlagsMetadata();
        llvm::Metadata *getVersionMetadata();
        llvm::MDNode *getCLVersionMetadata();

        llvm::MDBuilder *getMDBuilder() const { return _MDbuilder; }
        llvm::LLVMContext *getContext() const { return _context; }
        llvm::DIBuilder *getDIBuilder() const { return _DIBuilder; }

    public:
        Metadata(){};

        Metadata(llvm::LLVMContext *, llvm::Module &);

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
