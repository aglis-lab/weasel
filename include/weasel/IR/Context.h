#pragma once

#include <vector>
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
        std::vector<llvm::BasicBlock *> _breakBlocks;
        std::vector<llvm::BasicBlock *> _continueBlocks;
        std::unordered_map<std::string, llvm::StructType *> _structTypes;

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

        // Helper for User Defined Struct //
        inline void addStructType(const std::string &name, llvm::StructType *type) { _structTypes[name] = type; }
        inline llvm::StructType *findStructType(const std::string &name)
        {
            if (_structTypes.find(name) != _structTypes.end())
            {
                return _structTypes[name];
            }

            return nullptr;
        }

    public:
        Context(llvm::LLVMContext *context, const std::string &moduleName);

        llvm::LLVMContext *getContext() const { return _context; }
        llvm::Module *getModule() const { return _module; }
        llvm::IRBuilder<> *getBuilder() const { return _builder; }
        llvm::MDBuilder *getMDBuilder() const { return _mdBuilder; }

        std::string getDefaultLabel();

    public:
        llvm::Type *codegen(Type *type);
        llvm::Type *codegen(StructType *type);

        // Integer Fast Casting
        llvm::Value *castInteger(llvm::Value *val, llvm::Type *type, bool isSign = false)
        {
            if (isSign)
            {
                return getBuilder()->CreateSExtOrTrunc(val, type);
            }

            return getBuilder()->CreateZExtOrTrunc(val, type);
        }

        // Literal Expression
        llvm::Value *codegen(BoolLiteralExpression *expr) const;
        llvm::Value *codegen(CharLiteralExpression *expr) const;
        llvm::Value *codegen(NumberLiteralExpression *expr);
        llvm::Value *codegen(FloatLiteralExpression *expr) const;
        llvm::Value *codegen(DoubleLiteralExpression *expr) const;
        llvm::Value *codegen(StringLiteralExpression *expr) const;
        llvm::Value *codegen(ArrayLiteralExpression *expr);

        // Expression
        llvm::Value *codegen(StructExpression *expr);
        llvm::Value *codegen(VariableExpression *expr);
        llvm::Value *codegen(CallExpression *expr);
        llvm::Value *codegen(ReturnExpression *expr);
        llvm::Value *codegen(DeclarationStatement *expr);
        llvm::Value *codegen(NilLiteralExpression *expr) const;
        llvm::Value *codegen(ArrayExpression *expr);
        llvm::Value *codegen(FieldExpression *expr);

        // Condition Statement
        llvm::Value *codegen(ConditionStatement *expr);
        llvm::Value *codegen(CompoundStatement *expr);
        llvm::Value *codegen(LoopingStatement *expr);
        llvm::Value *codegen(BreakExpression *expr);
        llvm::Value *codegen(ContinueExpression *expr);

        // Operator Expression
        llvm::Value *codegen(ArithmeticExpression *expr);
        llvm::Value *codegen(LogicalExpression *expr);
        llvm::Value *codegen(AssignmentExpression *expr);
        llvm::Value *codegen(ComparisonExpression *expr);
        llvm::Value *codegen(UnaryExpression *expr);

        // User Defined
        llvm::Value *codegen(Function *func);
    };
} // namespace weasel
