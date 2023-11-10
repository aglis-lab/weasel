#pragma once

#include <weasel/AST/AST.h>
#include <weasel/Table/ContextTable.h>

#include <vector>
#include <map>

#include <llvm/IR/Value.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/MDBuilder.h>

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
    class WeaselCodegen : ContextTable
    {
    public:
        WeaselCodegen(llvm::LLVMContext *context, const std::string &moduleName);

        llvm::LLVMContext *getContext() const { return _context; }
        llvm::Module *getModule() const { return _module; }
        llvm::IRBuilder<> *getBuilder() const { return _builder; }
        llvm::MDBuilder *getMDBuilder() const { return _mdBuilder; }

    public:
        // Type
        llvm::Type *codegen(Type *type);
        llvm::Type *codegen(StructType *type);

        // Integer Fast Casting
        llvm::Value *castInteger(llvm::Value *val, llvm::Type *type, bool isSign = false);

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
        llvm::Value *codegen(NilLiteralExpression *expr);
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
        llvm::Value *codegen(TypeCastExpression *expr);

        // User Defined
        llvm::Value *codegen(Function *func);

    private:
        // Helper Builder //
        llvm::Module *_module;
        llvm::MDBuilder *_mdBuilder;
        llvm::LLVMContext *_context;
        llvm::IRBuilder<> *_builder;

        // Helper Variable for Looping //
        std::vector<llvm::BasicBlock *> _breakBlocks;
        std::vector<llvm::BasicBlock *> _continueBlocks;

        // Helper For Return Function //
        llvm::Value *_returnValue;
        llvm::BasicBlock *_returnBlock;

        // Helper Variable for Struct Types //
        std::map<std::string, llvm::StructType *> _structTypes;

    private:
        std::map<Expression *, llvm::AllocaInst *> _allocaMap;

        void traverseAllocaExpression(Expression *expr);

        void setAllocaMap(Expression *expr, llvm::AllocaInst *val)
        {
            _allocaMap[expr] = val;
        }
        llvm::AllocaInst *getAllocaMap(Expression *expr)
        {
            return _allocaMap[expr];
        }

    private:
        // MDNode
        llvm::MDNode *getTBAA(llvm::Type *type) const;
        llvm::MDNode *getTBAARoot() const;
        llvm::MDNode *getTBAAChar() const;
        llvm::MDNode *getTBAAShort() const;
        llvm::MDNode *getTBAAInt() const;
        llvm::MDNode *getTBAALong() const;
        llvm::MDNode *getTBAAPointer() const;

        // Helper for Break looping //
        void addbreakBlock(llvm::BasicBlock *block) { _breakBlocks.push_back(block); }
        void removeBreakBlock() { _breakBlocks.pop_back(); }
        bool isBreakBlockExist() const { return !_breakBlocks.empty(); }
        llvm::BasicBlock *getBreakBlock() const { return _breakBlocks.back(); }

        // Helper for Continue Looping //
        void addContinueBlock(llvm::BasicBlock *block) { _continueBlocks.push_back(block); }
        void removeContinueBlock() { _continueBlocks.pop_back(); }
        bool isContinueBlockExist() const { return !_continueBlocks.empty(); }
        llvm::BasicBlock *getContinueBlock() const { return _continueBlocks.back(); }

        // Helper for User Defined Struct //
        void addStructType(const std::string &name, llvm::StructType *type) { _structTypes[name] = type; }
        llvm::StructType *findStructType(const std::string &name)
        {
            if (_structTypes.find(name) != _structTypes.end())
            {
                return _structTypes[name];
            }

            return nullptr;
        }
    };
} // namespace weasel
