#pragma once

#include <list>
#include <unordered_map>

#include <llvm/IR/Value.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/MDBuilder.h>

#include "weasel/AST/AST.h"
#include "weasel/Table/ContextTable.h"

namespace weasel
{
    // Compare Type
    enum class CompareType
    {
        Equal,
        Casting,
        Different,
    };

    // Analysis Context
    class Context : ContextTable
    {
    private:
        llvm::Module *_module;
        llvm::MDBuilder *_mdBuilder;
        llvm::LLVMContext *_context;
        llvm::IRBuilder<> *_builder;
        Function *_currentFunction = nullptr;
        unsigned long _counter = 0;

        // Helper for Looping //
        std::list<llvm::BasicBlock *> _breakBlocks;
        std::list<llvm::BasicBlock *> _continueBlocks;
        std::unordered_map<std::string, StructType> _structTypes;

    private:
        llvm::MDNode *getTBAA(llvm::Type *type) const;
        llvm::MDNode *getTBAARoot() const;
        llvm::MDNode *getTBAAChar() const;
        llvm::MDNode *getTBAAShort() const;
        llvm::MDNode *getTBAAInt() const;
        llvm::MDNode *getTBAALong() const;
        llvm::MDNode *getTBAAPointer() const;

        // Helper for Break looping //
        inline void addbreakBlock(llvm::BasicBlock *block) { _breakBlocks.push_back(block); }
        inline void removeBreakBlock() { _breakBlocks.pop_back(); }
        inline bool isBreakBlockExist() const { return !_breakBlocks.empty(); }
        inline llvm::BasicBlock *getBreakBlock() const { return _breakBlocks.back(); }

        // Helper for Continue Looping //
        inline void addContinueBlock(llvm::BasicBlock *block) { _continueBlocks.push_back(block); }
        inline void removeContinueBlock() { _continueBlocks.pop_back(); }
        inline bool isContinueBlockExist() const { return !_continueBlocks.empty(); }
        inline llvm::BasicBlock *getContinueBlock() const { return _continueBlocks.back(); }

    public:
        explicit Context(llvm::LLVMContext *context, const std::string &moduleName);

        llvm::LLVMContext *getContext() const { return _context; }
        llvm::Module *getModule() const { return _module; }
        llvm::IRBuilder<> *getBuilder() const { return _builder; }
        llvm::MDBuilder *getMDBuilder() const { return _mdBuilder; }

        llvm::Value *castIntegerType(llvm::Value *lhs, llvm::Value *rhs) const;
        llvm::Value *castIntegerType(llvm::Value *value, llvm::Type *castTy);
        CompareType compareType(llvm::Type *lhsType, llvm::Type *rhsType);
        std::string getDefaultLabel();

    public:
        llvm::Type *codegen(Type *type);
        llvm::Type *codegen(StructType *type);

        llvm::Value *codegen(BoolLiteralExpression *expr) const;
        llvm::Value *codegen(CharLiteralExpression *expr) const;
        llvm::Value *codegen(NumberLiteralExpression *expr) const;
        llvm::Value *codegen(FloatLiteralExpression *expr) const;
        llvm::Value *codegen(DoubleLiteralExpression *expr) const;
        llvm::Value *codegen(StringLiteralExpression *expr) const;
        llvm::Value *codegen(ArrayLiteralExpression *expr);

        // Expression
        llvm::Value *codegen(VariableExpression *expr);
        llvm::Value *codegen(MethodCallExpression *expr);
        llvm::Value *codegen(ReturnExpression *expr);
        llvm::Value *codegen(DeclarationExpression *expr);
        llvm::Value *codegen(NilLiteralExpression *expr) const;
        llvm::Value *codegen(ArrayExpression *expr);

        // Condition Statement
        llvm::Value *codegen(ConditionStatement *expr);
        llvm::Value *codegen(StatementExpression *expr);
        llvm::Value *codegen(LoopingStatement *expr);
        llvm::Value *codegen(BreakExpression *expr);
        llvm::Value *codegen(ContinueExpression *expr);
        llvm::Value *codegen(BinaryOperatorExpression *expr);

        // User Defined
        llvm::Value *codegen(Function *func);
    };
} // namespace weasel
