//
// Created by zaen on 27/06/21.
//
#pragma once

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/TargetTransformInfo.h>

using namespace std;

namespace weasel
{
    class Passes
    {
    private:
        unique_ptr<llvm::legacy::FunctionPassManager> _fpm;

    public:
        Passes(llvm::Module *module);

        bool run(llvm::Function &fun);
    };
}
