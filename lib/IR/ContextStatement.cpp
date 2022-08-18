#include <iostream>
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"

llvm::Value *weasel::Context::codegen(DeclarationStatement *expr)
{
    // Get Value Representation
    auto declType = expr->getType();
    auto valueExpr = expr->getValue();

    if (declType == nullptr && valueExpr != nullptr && valueExpr->getType() != nullptr)
    {
        declType = valueExpr->getType();
        expr->setType(declType);
    }

    // Allocating Address for declaration
    auto varName = expr->getIdentifier();
    auto declTypeV = declType->codegen(this);
    if (declTypeV == nullptr)
    {
        return ErrorTable::addError(expr->getToken(), "Unexpected error when codegen a type");
    }

    // Default Value
    if (valueExpr == nullptr)
    {
        auto alloc = getBuilder()->CreateAlloca(declTypeV, nullptr);
        llvm::Constant *constantVal = nullptr;

        // Default Value for integer
        if (declType->isIntegerType())
        {
            auto constantVal = llvm::ConstantInt::get(declTypeV, 0, declType->isSigned());
        }

        // Default Value for Float
        if (declType->isFloatType())
        {
            auto constantVal = llvm::ConstantFP::get(declTypeV, 0);
        }

        // Store Default Value
        if (constantVal != nullptr)
        {
            getBuilder()->CreateStore(constantVal, alloc);
        }

        // Add Variable Declaration to symbol table
        addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

        return nullptr;
    }

    auto valueType = valueExpr->getType();
    if (valueType->isVoidType())
    {
        return ErrorTable::addError(valueExpr->getToken(), "Cannot assign void to a variable");
    }

    // Check if type is different
    // TODO: Change this to Analysis type checking
    if (!valueType->isEqual(declType))
    {
        return ErrorTable::addError(valueExpr->getToken(), "Cannot assign to different type");
    }

    auto valueV = valueExpr->codegen(this);
    if (valueV == nullptr)
    {
        return ErrorTable::addError(valueExpr->getToken(), "Cannot codegen value expression");
    }

    // TODO: LLVM Declare Struct Metadata
    // call void @llvm.dbg.declare(metadata %struct.Person* %3, metadata !20, metadata !DIExpression()), !dbg !28
    if (declType->isStructType())
    {
        auto widthVal = dynamic_cast<StructType *>(declType)->getStructTypeWidth();
        auto alloc = valueV;
        if (llvm::dyn_cast<llvm::Constant>(valueV) != nullptr)
        {
            alloc = getBuilder()->CreateAlloca(declTypeV, nullptr);
            getBuilder()->CreateMemSet(alloc, valueV, widthVal, llvm::MaybeAlign(0));
        }

        // Add Variable Declaration to symbol table
        addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

        return nullptr;
    }

    if (declType->isPrimitiveType() && declType->getTypeWidth() != valueType->getTypeWidth())
    {
        valueV = getBuilder()->CreateSExtOrTrunc(valueV, declTypeV);
    }

    auto alloc = getBuilder()->CreateAlloca(declTypeV, nullptr);
    getBuilder()->CreateStore(valueV, alloc);

    // Add Variable Declaration to symbol table
    addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

    return nullptr;
}

llvm::Value *weasel::Context::codegen(CompoundStatement *expr)
{
    // Enter to new statement
    enterScope();

    for (auto &item : expr->getBody())
    {
        item->codegen(this);
    }

    // Exit from statement
    exitScope();

    return nullptr;
}

llvm::Value *weasel::Context::codegen(ConditionStatement *expr)
{
    auto conditions = expr->getConditions();
    auto statements = expr->getStatements();
    auto count = conditions.size();
    auto parentFun = getBuilder()->GetInsertBlock()->getParent();
    auto endBlock = llvm::BasicBlock::Create(*getContext());

    for (int i = 0; i < count; i++)
    {
        auto condition = conditions[i];
        auto statement = statements[i];
        auto conditionType = condition->getType();
        if (!conditionType->isBooleanType())
        {
            return ErrorTable::addError(condition->getToken(), "Expected Boolean Type");
        }

        auto currentBlock = getBuilder()->GetInsertBlock();
        auto bodyBlock = llvm::BasicBlock::Create(*getContext(), "", parentFun);
        auto nextBlock = llvm::BasicBlock::Create(*getContext());

        // Create Condition Branch
        getBuilder()->CreateCondBr(condition->codegen(this), bodyBlock, nextBlock);

        // Set Insert Point
        getBuilder()->SetInsertPoint(bodyBlock);

        // Codegen Body
        statement->codegen(this);

        // Jump to Next Block
        if (!getBuilder()->GetInsertBlock()->back().isTerminator())
        {
            getBuilder()->CreateBr(endBlock);
        }

        // Add Next Block to Fuction
        parentFun->getBasicBlockList().push_back(nextBlock);

        // Set Insert point
        getBuilder()->SetInsertPoint(nextBlock);
    }

    if (expr->isElseExist())
    {
        auto statement = statements.back();
        auto elseBlock = llvm::BasicBlock::Create(*getContext(), "", parentFun);

        getBuilder()->CreateBr(elseBlock);
        getBuilder()->SetInsertPoint(elseBlock);

        statement->codegen(this);
    }

    // Jump to Next Block
    if (!getBuilder()->GetInsertBlock()->back().isTerminator())
    {
        getBuilder()->CreateBr(endBlock);
    }

    // Add End Block to Fuction
    parentFun->getBasicBlockList().push_back(endBlock);

    // Set Insert point
    getBuilder()->SetInsertPoint(endBlock);

    // TODO: Calculate PHI

    return nullptr;
}

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
    enterScope();

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
    exitScope();

    return nullptr;
}
