#pragma once

#include <weasel/AST/AST.h>
#include <weasel/Metadata/Metadata.h>

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
    class Codegen
    {
        // Weasel Package
    private:
        Metadata _metaData;

    public:
        Codegen(llvm::LLVMContext *context, const string &moduleName);

        // Builder
        llvm::LLVMContext *getContext() const { return _context; }
        llvm::Module *getModule() { return _module; }
        llvm::IRBuilder<> *getBuilder() { return _builder; }
        llvm::MDBuilder *getMDBuilder() { return _mdBuilder; }

        // Type
        llvm::Type *codegen(Type *type);
        llvm::Type *codegen(StructType *type);
        llvm::Type *codegen(FunctionType *type);

        // Integer Fast Casting
        llvm::Value *castInteger(llvm::Value *val, llvm::Type *type, bool isSign = false);

        // Literal Expression
        llvm::Value *codegen(BoolLiteralExpression *expr);
        llvm::Value *codegen(CharLiteralExpression *expr);
        llvm::Value *codegen(NumberLiteralExpression *expr);
        llvm::Value *codegen(FloatLiteralExpression *expr);
        llvm::Value *codegen(DoubleLiteralExpression *expr);
        llvm::Value *codegen(StringLiteralExpression *expr);
        llvm::Value *codegen(ArrayExpression *expr);
        llvm::Value *codegen(GlobalVariable *expr);

        // Expression
        llvm::Value *codegen(StructExpression *expr);
        llvm::Value *codegen(VariableExpression *expr);
        llvm::Value *codegen(CallExpression *expr);
        llvm::Value *codegen(ReturnExpression *expr);
        llvm::Value *codegen(DeclarationStatement *expr);
        llvm::Value *codegen(NilLiteralExpression *expr);
        llvm::Value *codegen(IndexExpression *expr);
        llvm::Value *codegen(FieldExpression *expr);
        llvm::Value *codegen(MethodCallExpression *expr);

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
        llvm::LLVMContext *_context;
        llvm::Module *_module;
        llvm::MDBuilder *_mdBuilder;
        llvm::IRBuilder<> *_builder;

        // Helper Variable for Looping //
        vector<llvm::BasicBlock *> _breakBlocks;
        vector<llvm::BasicBlock *> _continueBlocks;

        // Helper For Return Function //
        llvm::AllocaInst *_returnAlloca;
        llvm::BasicBlock *_returnBlock;
        llvm::BasicBlock *_allocaBlock;

        // Helper Variable for Struct Types and Functions //
        map<string, llvm::StructType *> _structTypes;
        map<string, llvm::Function *> _functions;

    private:
        // Create allocation instruction at alloca block
        llvm::AllocaInst *createAlloca(llvm::Type *type, unsigned int addrSpace = 0, const llvm::Twine &name = "")
        {
            return new llvm::AllocaInst(type, addrSpace, name, _allocaBlock);
        }

    private:
        // MDNode
        llvm::MDNode *getTBAA(llvm::Type *type);
        llvm::MDNode *getTBAARoot();
        llvm::MDNode *getTBAAChar();
        llvm::MDNode *getTBAAShort();
        llvm::MDNode *getTBAAInt();
        llvm::MDNode *getTBAALong();
        llvm::MDNode *getTBAAPointer();

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
        void addStructType(const string &name, llvm::StructType *type) { _structTypes[name] = type; }
        llvm::StructType *findStructType(const string &name)
        {
            if (_structTypes.contains(name))
            {
                return _structTypes[name];
            }

            return nullptr;
        }
    };
} // namespace weasel
