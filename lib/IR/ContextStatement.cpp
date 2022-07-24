#include <iostream>
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"

llvm::Value *weasel::Context::codegen(StatementExpression *expr)
{
    // Enter to new statement
    {
        SymbolTable::enterScope();
    }

    for (auto &item : expr->getBody())
    {
        auto val = item->codegen(this);
    }

    // Exit from statement
    {
        SymbolTable::exitScope();
    }

    return nullptr;
}

llvm::Value *weasel::Context::codegen(ConditionStatement *expr)
{
    auto cond = expr->getCondition();
    auto body = expr->getBody();
    auto condType = cond->getType();

    if (!condType->isBooleanType())
    {
        return ErrorTable::addError(cond->getToken(), "Expected Boolean Type");
    }

    auto currentBlock = getBuilder()->GetInsertBlock();
    auto parentFun = currentBlock->getParent();
    auto bodyBlock = llvm::BasicBlock::Create(*this->getContext(), "", parentFun);
    auto lastBlock = llvm::BasicBlock::Create(*this->getContext(), "", parentFun);

    // Create Condition Branch
    auto condVal = cond->codegen(this);
    llvm::BranchInst::Create(bodyBlock, lastBlock, condVal, currentBlock);

    // Change to If Body Block
    getBuilder()->SetInsertPoint(bodyBlock);

    // Codegen Body
    body->codegen(this);
    if (!getBuilder()->GetInsertBlock()->back().isTerminator())
    {
        getBuilder()->CreateBr(lastBlock);
    }

    // Emit Then Branch
    getBuilder()->SetInsertPoint(lastBlock);

    // TODO: Calculate PHI

    return nullptr;
}

// TODO: Need More type of for variations
llvm::Value *weasel::Context::codegen(LoopingStatement *expr)
{
    auto isInfinity = expr->isInfinityCondition();
    auto isSingleCondition = expr->isSingleCondition();
    auto currentBlock = getBuilder()->GetInsertBlock();
    auto bodyBlock = llvm::BasicBlock::Create(*getContext());
    auto endBlock = llvm::BasicBlock::Create(*getContext());
    auto initialBlock = llvm::BasicBlock::Create(*getContext());
    auto conditionBlock = llvm::BasicBlock::Create(*getContext());
    auto countBlock = llvm::BasicBlock::Create(*getContext());

    auto parentFun = currentBlock->getParent();
    auto conditions = expr->getConditions();

    // Enter to new statement
    {
        SymbolTable::enterScope();
    }

    // Add Last Block to Loop Blocks
    addbreakBlock(endBlock);
    addContinueBlock(conditionBlock);

    // Initial //
    if (!isInfinity && !isSingleCondition)
    {
        auto initialExpr = conditions[0];
        initialExpr->codegen(this);
    }

    // Condition //
    // Jump to Conditional
    getBuilder()->CreateBr(conditionBlock);

    // Set Insert point to Conditional Block
    parentFun->getBasicBlockList().push_back(conditionBlock);
    getBuilder()->SetInsertPoint(conditionBlock);
    if (!isInfinity)
    {
        auto conditionExpr = isSingleCondition ? conditions[0] : conditions[1];

        if (!conditionExpr->getType()->isBooleanType())
        {
            return ErrorTable::addError(conditionExpr->getToken(), "Expected Boolean Type for Looping Condition");
        }

        getBuilder()->CreateCondBr(conditionExpr->codegen(this), bodyBlock, endBlock);
    }
    else
    {
        // If Infinity just jump to body block
        getBuilder()->CreateBr(bodyBlock);
    }

    // Block //
    // Set Insert Point to body block
    parentFun->getBasicBlockList().push_back(bodyBlock);
    getBuilder()->SetInsertPoint(bodyBlock);

    // Codegen Body
    expr->getBody()->codegen(this);

    // Counting //
    // Jump to Counting
    getBuilder()->CreateBr(countBlock);

    // Set Insert Point to Counting
    parentFun->getBasicBlockList().push_back(countBlock);
    getBuilder()->SetInsertPoint(countBlock);

    // Check if counting expression found
    if (!isInfinity && !isSingleCondition)
    {
        auto countExpr = conditions[2];

        countExpr->codegen(this);
    }

    // Jump back to Condition
    getBuilder()->CreateBr(conditionBlock);

    // End Block //
    parentFun->getBasicBlockList().push_back(endBlock);
    getBuilder()->SetInsertPoint(endBlock);

    removeBreakBlock();
    removeContinueBlock();

    // Exit from statement
    {
        SymbolTable::exitScope();
    }

    return nullptr;
}
