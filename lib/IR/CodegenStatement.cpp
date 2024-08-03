#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/IR/Function.h>

#include "weasel/IR/Codegen.h"

llvm::Value *Codegen::codegen(GlobalVariable *expr)
{
    return nullptr;
}

llvm::Value *Codegen::castInteger(llvm::Value *val, llvm::Type *type, bool isSign)
{
    if (isSign)
    {
        return getBuilder()->CreateSExtOrTrunc(val, type);
    }

    return getBuilder()->CreateZExtOrTrunc(val, type);
}

llvm::Value *Codegen::codegen(Function *expr)
{
    LOG(INFO) << "Codegen Function " << expr->getIdentifier();

    auto isVararg = expr->isVararg();
    auto args = expr->getArguments();

    auto argsLength = (int)args.size() - (isVararg ? 1 : 0);
    auto argsV = std::vector<llvm::Type *>(argsLength);
    for (int index = 0; index < argsLength; index++)
    {
        argsV[index] = args[index]->getType()->accept(this);
    }

    auto returnType = expr->getReturnType();
    auto returnTypeV = returnType->accept(this);
    auto linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
    auto funTypeV = llvm::FunctionType::get(returnTypeV, argsV, isVararg);
    auto funV = llvm::Function::Create(funTypeV, linkage, expr->getManglingName(), getModule());

    funV->addFnAttr(llvm::Attribute::AttrKind::InlineHint);
    funV->addFnAttr(llvm::Attribute::AttrKind::AlwaysInline);
    funV->setDSOLocal(true);

    // Save The Function
    expr->setCodegen(funV);

    if (expr->isDefine())
    {
        // Create Alloca Block
        _allocaBlock = llvm::BasicBlock::Create(*getContext(), "", funV);

        // Create Entry Point
        auto entryBlock = llvm::BasicBlock::Create(*getContext(), "entry", funV);
        getBuilder()->SetInsertPoint(entryBlock);

        // Enter to new scope
        enterScope();

        // Allocate Arguments
        struct AllocateArgument
        {
            llvm::AllocaInst *alloca;
            llvm::Argument *arg;
        };

        std::vector<AllocateArgument> argAllocs;
        for (size_t i = 0; i < args.size(); i++)
        {
            auto argExpr = args[i];
            auto argName = argExpr->getIdentifier();
            auto item = funV->getArg(i);

            item->setName(argName);

            // Allocate Argument
            llvm::Value *value;
            if (argExpr->getType()->isPointerType() || argExpr->getType()->isReferenceType())
            {
                value = item;
            }
            else
            {
                auto alloc = createAlloca(item->getType());

                argAllocs.push_back(AllocateArgument{alloc, item});
                value = alloc;
            }

            // Add Attribute
            addAttribute(ContextAttribute::get(argName, value, AttributeKind::Parameter));
        }

        // Create Block
        LOG(INFO) << "Codegen Function Body";

        // Store argument value
        for (auto item : argAllocs)
        {
            getBuilder()->CreateStore(item.arg, item.alloca);
        }

        // Create return block
        _returnBlock = llvm::BasicBlock::Create(*getContext());

        // Create Return Block and Value
        if (!returnType->isVoidType())
        {
            _returnAlloca = getBuilder()->CreateAlloca(funTypeV->getReturnType());
        }

        // Codegen Function Body
        expr->getBody()->accept(this);

        // Create Default Return Block
        if (llvm::dyn_cast<llvm::BranchInst>(&getBuilder()->GetInsertBlock()->back()) == nullptr)
        {
            getBuilder()->CreateBr(_returnBlock);
        }

        // Evaluate Return
        funV->insert(funV->end(), _returnBlock);
        getBuilder()->SetInsertPoint(_returnBlock);

        if (returnType->isVoidType())
        {
            getBuilder()->CreateRetVoid();
        }
        else
        {
            auto loadReturnValue = getBuilder()->CreateLoad(funTypeV->getReturnType(), _returnAlloca);
            getBuilder()->CreateRet(loadReturnValue);
        }

        // Merge entry block into allocablock
        llvm::BranchInst::Create(entryBlock, _allocaBlock);
        auto mergeEntryBlock = llvm::MergeBlockIntoPredecessor(entryBlock);
        assert(mergeEntryBlock && "Merge Entry Block into alloca block");

        // Release Return Block and Value
        _returnAlloca = nullptr;
        _returnBlock = nullptr;

        // Exit fromscope
        exitScope();
    }

    return funV;
}

llvm::Value *Codegen::codegen(MethodCallExpression *expr)
{
    LOG(INFO) << "Codegen Method Call Function\n";

    auto argsFun = expr->getFunction()->getArguments();
    auto argsCall = expr->getArguments();

    argsCall.insert(argsCall.begin(), expr->getImplExpression());

    std::vector<llvm::Value *> argsV;
    for (size_t i = 0; i < argsCall.size(); i++)
    {
        auto argCall = argsCall[i];

        if (auto argFun = argsFun[i]; argFun->getType()->isReferenceType())
        {
            argCall->setAccess(AccessID::Allocation);
        }
        else
        {
            argCall->setAccess(AccessID::Load);
        }

        auto argVal = argCall->accept(this);
        assert(!argVal && "failed codegen argument");

        argsV.push_back(argVal);
    }

    auto fun = getModule()->getFunction(expr->getFunction()->getManglingName());
    return getBuilder()->CreateCall(fun, argsV);
}

llvm::Value *Codegen::codegen(CallExpression *expr)
{
    LOG(INFO) << "Codegen Call Function";

    auto mangleName = expr->getFunction()->getManglingName();
    auto args = expr->getArguments();
    auto fun = getModule()->getFunction(mangleName);
    auto funArgs = expr->getFunction()->getArguments();

    std::vector<llvm::Value *> argsV;
    for (size_t i = 0; i < args.size(); i++)
    {
        auto arg = args[i];
        if (arg->getType()->isReferenceType())
        {
            arg->setAccess(AccessID::Allocation);
        }
        else
        {
            arg->setAccess(AccessID::Load);
        }

        auto argVal = arg->accept(this);
        assert(argVal && "failed codegen argument");

        argsV.push_back(argVal);
    }

    return getBuilder()->CreateCall(fun, argsV);
}

llvm::Value *Codegen::codegen(BreakExpression *expr)
{
    assert(isBreakBlockExist() && "No looping found");

    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateBr(getBreakBlock());
    }

    auto condExpr = expr->getValue();
    auto condType = condExpr->getType();

    assert(condType->isBoolType() && "Break Condition should be boolean");

    auto newBlock = llvm::BasicBlock::Create(*getContext(), "", getBuilder()->GetInsertBlock()->getParent());
    auto breakBlock = getBreakBlock();
    auto condVal = condExpr->accept(this);
    auto brIns = getBuilder()->CreateCondBr(condVal, breakBlock, newBlock);

    // Create New Insert Point
    getBuilder()->SetInsertPoint(newBlock);

    return brIns;
}

llvm::Value *Codegen::codegen(ContinueExpression *expr)
{
    assert(isContinueBlockExist() && "No looping found");

    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateBr(getContinueBlock());
    }

    auto condExpr = expr->getValue();
    auto condType = condExpr->getType();

    assert(condType->isBoolType() && "Continue Condition should be boolean");

    auto newBlock = llvm::BasicBlock::Create(*getContext(), "", getBuilder()->GetInsertBlock()->getParent());
    auto continueBlock = getContinueBlock();
    auto condVal = condExpr->accept(this);
    auto brIns = getBuilder()->CreateCondBr(condVal, continueBlock, newBlock);

    // // Create New Insert Point
    getBuilder()->SetInsertPoint(newBlock);

    return brIns;
}

llvm::Value *Codegen::codegen(ReturnExpression *expr)
{
    LOG(INFO) << "Codegen Return Function\n";

    if (!expr->getType()->isVoidType())
    {
        auto val = expr->getValue()->accept(this);
        if (expr->getType()->isIntegerType())
        {
            auto returnTyV = getBuilder()->GetInsertBlock()->getParent()->getReturnType();

            if (expr->getType()->isSigned())
            {
                val = getBuilder()->CreateSExtOrTrunc(val, returnTyV);
            }
            else
            {
                val = getBuilder()->CreateZExtOrTrunc(val, returnTyV);
            }
        }

        getBuilder()->CreateStore(val, _returnAlloca);
    }

    return getBuilder()->CreateBr(_returnBlock);
}

llvm::Value *Codegen::codegen(VariableExpression *expr)
{
    LOG(INFO) << "Codegen Variable";

    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto type = expr->getType();
    auto typeV = type->accept(this);

    fmt::println("TEST {}", expr->getIdentifier());
    llvm::Value *alloc;
    if (type->isFunctionType())
    {
        alloc = expr->getDeclarationValue()->getCodegen();
    }
    else
    {
        auto attr = findAttribute(varName);

        assert(!attr.isEmpty() && "value of the variable isn't found");

        alloc = attr.getValue();
    }

    assert(alloc && "variable isn't declare yet");

    fmt::println("TEST {} {}", expr->getIdentifier(), (alloc == nullptr));

    if (llvm::dyn_cast<llvm::Argument>(alloc))
    {
        fmt::println("TEST {}", expr->getIdentifier());
        return alloc;
    }

    fmt::println("TEST {}", expr->getIdentifier());

    if (type->isArrayType())
    {
        return getBuilder()->CreateInBoundsGEP(typeV, alloc, {getBuilder()->getInt64(0), getBuilder()->getInt64(0)});
    }

    if (expr->isAccessAllocation())
    {
        fmt::println("TEST Alloc {}", expr->getIdentifier());
        return alloc;
    }

    return getBuilder()->CreateLoad(typeV, alloc);
}

// TODO: Pointer should be opaque type
// TODO: String as array of byte
llvm::Value *Codegen::codegen(ArrayExpression *expr)
{
    LOG(INFO) << "Codegen Array Expression";

    return nullptr;

    // // Get Index
    // auto indexExpr = expr->getIndex();
    // auto indexV = indexExpr->accept(this);

    // assert(indexV);
    // assert(indexExpr);

    // // Get Allocator from Symbol Table
    // auto varName = expr->getIdentifier();
    // auto attr = findAttribute(varName);

    // assert(!attr.isEmpty() && "Cannot find variable");

    // auto alloc = attr.getValue();

    // assert(alloc);

    // // Casting to integer 64
    // indexV = getBuilder()->CreateSExtOrTrunc(indexV, getBuilder()->getInt64Ty());

    // std::vector<llvm::Value *> idxList = {indexV};

    // // TODO: getPointerElementType deprecated
    // auto typeV = alloc->getType()->getPointerElementType();
    // if (typeV->isPointerTy())
    // {
    //     alloc = getBuilder()->CreateLoad(typeV, alloc);
    //     typeV = typeV->getPointerElementType();
    // }
    // else
    // {
    //     idxList.insert(idxList.begin(), getBuilder()->getInt64(0));
    // }

    // auto elemIndex = getBuilder()->CreateInBoundsGEP(typeV, alloc, idxList);
    // if (expr->isAccessAllocation())
    // {
    //     return elemIndex;
    // }

    // return getBuilder()->CreateLoad(typeV, elemIndex);
}

llvm::Value *Codegen::codegen(FieldExpression *expr)
{
    LOG(INFO) << "Codegen Field Expression";

    expr->getParentField()->setAccess(AccessID::Allocation);
    auto alloc = expr->getParentField()->accept(this);

    assert(alloc && "allocation should be exist");

    auto type = static_pointer_cast<StructType>(expr->getParentField()->getType());
    auto typeV = type->accept(this);

    auto fieldName = expr->getIdentifier();
    auto [idx, field] = type->findTypeName(fieldName);

    auto range = getBuilder()->getInt32(0);
    auto idxVal = getBuilder()->getInt32(idx);
    auto inbound = getBuilder()->CreateInBoundsGEP(typeV, alloc, {range, idxVal});
    if (expr->isAccessAllocation())
    {
        return inbound;
    }

    auto fieldV = field->getType()->accept(this);
    return getBuilder()->CreateLoad(fieldV, inbound);
}

llvm::Value *Codegen::codegen(StructExpression *expr)
{
    LOG(INFO) << "Codegen Struct Expression...";
    if (expr->getFields().empty())
    {
        return getBuilder()->getInt8(0);
    }

    auto type = static_pointer_cast<StructType>(expr->getType());
    auto typeV = type->accept(this);
    auto alloc = expr->getAlloc();
    auto fields = type->getFields();
    for (auto exprField : expr->getFields())
    {
        auto [idx, exprFieldType] = type->findTypeName(exprField->getIdentifier());

        assert(idx >= 0 && "expression field should be exist");

        auto idxStruct = getBuilder()->getInt32(0);
        auto idxVal = getBuilder()->getInt32(idx);
        auto inbound = getBuilder()->CreateInBoundsGEP(typeV, alloc, {idxStruct, idxVal});
        if (exprField->getValue()->isStructExpression())
        {
            auto temp = static_pointer_cast<StructExpression>(exprField->getValue());
            temp->setAlloc(inbound);
            temp->accept(this);
        }
        else
        {
            auto val = exprField->getValue()->accept(this);
            getBuilder()->CreateStore(val, inbound);
        }
    }

    return alloc;
}

llvm::Value *Codegen::codegen(DeclarationStatement *expr)
{
    LOG(INFO) << "Codegen Declaration Statement";

    // Get Value Representation
    auto declType = expr->getType();
    auto valueExpr = expr->getValue();

    // Allocating Address for declaration
    auto varName = expr->getIdentifier();
    auto declTypeV = declType->accept(this);

    // Create Alloca
    auto alloc = createAlloca(declTypeV);

    // Save Allocation
    fmt::println("ALLOCATE {}", expr->getIdentifier());
    expr->setCodegen(alloc);

    // Set Default Value if no value expression
    if (valueExpr == nullptr)
    {
        // Add Variable Declaration to symbol table
        addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

        return alloc;
    }

    // Check if type isn't void
    auto valueType = valueExpr->getType();
    assert(!valueType->isVoidType() && "Cannot assign void to a variable");

    // Check if type is different
    assert(valueType->isEqual(declType) && "Cannot assign to different type");

    // Check if StructExpression
    if (valueExpr->isStructExpression())
    {
        auto temp = static_pointer_cast<StructExpression>(valueExpr);
        temp->setAlloc(alloc);
        temp->accept(this);

        // Add Variable Declaration to symbol table
        addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

        return alloc;
    }

    // Codegen Value Expression
    auto valueV = valueExpr->accept(this);
    assert(valueV && "Cannot codegen value expression");

    // Check if Array Literal
    if (typeid(*valueExpr.get()) == typeid(ArrayLiteralExpression))
    {
        auto arrayPtr = getBuilder()->CreateBitCast(alloc, getBuilder()->getInt8PtrTy());
        auto align = declType->getContainedType()->getTypeWidthByte();
        auto size = align * valueType->getTypeWidth();

        getBuilder()->CreateMemCpy(arrayPtr, llvm::MaybeAlign(align), valueV, llvm::MaybeAlign(align), getBuilder()->getInt64(size));
    }
    else
    {
        getBuilder()->CreateStore(valueV, alloc);
    }

    // Add Variable Declaration to symbol table
    addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

    return alloc;
}

llvm::Value *Codegen::codegen(CompoundStatement *expr)
{
    LOG(INFO) << "Codegen Compound Statement";

    // Enter to new statement
    enterScope();

    for (auto &item : expr->getBody())
    {
        item->setAccess(AccessID::Allocation);
        item->accept(this);
    }

    // Exit from statement
    exitScope();

    return nullptr;
}

llvm::Value *Codegen::codegen(ConditionStatement *expr)
{
    auto conditions = expr->getConditions();
    auto statements = expr->getStatements();
    auto count = (int)conditions.size();
    auto parentFun = getBuilder()->GetInsertBlock()->getParent();
    auto endBlock = llvm::BasicBlock::Create(*getContext());

    for (int i = 0; i < count; i++)
    {
        auto condition = conditions[i];
        auto statement = statements[i];
        auto conditionType = condition->getType();

        assert(conditionType->isBoolType() && "Expected Boolean Type");

        auto bodyBlock = llvm::BasicBlock::Create(*getContext(), "", parentFun);
        auto nextBlock = llvm::BasicBlock::Create(*getContext());

        // Create Condition Branch
        getBuilder()->CreateCondBr(condition->accept(this), bodyBlock, nextBlock);

        // Set Insert Point
        getBuilder()->SetInsertPoint(bodyBlock);

        // Driver Body
        statement->accept(this);

        // Jump to Next Block
        if (!getBuilder()->GetInsertBlock()->back().isTerminator())
        {
            getBuilder()->CreateBr(endBlock);
        }

        // Add Next Block to Fuction
        parentFun->insert(parentFun->end(), nextBlock);

        // Set Insert point
        getBuilder()->SetInsertPoint(nextBlock);
    }

    if (expr->isElseExist())
    {
        auto statement = statements.back();
        auto elseBlock = llvm::BasicBlock::Create(*getContext(), "", parentFun);

        getBuilder()->CreateBr(elseBlock);
        getBuilder()->SetInsertPoint(elseBlock);

        statement->accept(this);
    }

    // Jump to Next Block
    if (!getBuilder()->GetInsertBlock()->back().isTerminator())
    {
        getBuilder()->CreateBr(endBlock);
    }

    // Add End Block to Fuction
    parentFun->insert(parentFun->end(), endBlock);

    // Set Insert point
    getBuilder()->SetInsertPoint(endBlock);

    return nullptr;
}

llvm::Value *Codegen::codegen(LoopingStatement *expr)
{
    LOG(INFO) << "Codegen For Loop Statement";

    auto isInfinity = expr->isInfinityCondition();
    auto isSingleCondition = expr->isSingleCondition();
    auto currentBlock = getBuilder()->GetInsertBlock();
    auto bodyBlock = llvm::BasicBlock::Create(*getContext());
    auto endBlock = llvm::BasicBlock::Create(*getContext());
    auto conditionBlock = llvm::BasicBlock::Create(*getContext());
    auto countBlock = llvm::BasicBlock::Create(*getContext());
    auto parentFun = currentBlock->getParent();
    auto conditions = expr->getConditions();

    // Make Sure every variable or expression have LHS Meta Data
    for (auto item : conditions)
    {
        if (item != nullptr)
        {
            item->setAccess(AccessID::Allocation);
        }
    }

    // Enter to new statement
    enterScope();

    // Add Last Block to Loop Blocks
    addbreakBlock(endBlock);
    addContinueBlock(conditionBlock);

    // Initial //
    if (!isInfinity && !isSingleCondition)
    {
        auto initialExpr = conditions[0];
        initialExpr->accept(this);
    }

    // Condition //
    // Jump to Conditional
    getBuilder()->CreateBr(conditionBlock);

    // Set Insert point to Conditional Block
    parentFun->insert(parentFun->end(), conditionBlock);
    getBuilder()->SetInsertPoint(conditionBlock);
    if (!isInfinity)
    {
        auto conditionExpr = isSingleCondition ? conditions[0] : conditions[1];

        assert(conditionExpr->getType()->isBoolType() && "Expected Boolean Type for Looping Condition");

        getBuilder()->CreateCondBr(conditionExpr->accept(this), bodyBlock, endBlock);
    }
    else
    {
        // If Infinity just jump to body block
        getBuilder()->CreateBr(bodyBlock);
    }

    // Block //
    // Set Insert Point to body block
    parentFun->insert(parentFun->end(), bodyBlock);
    getBuilder()->SetInsertPoint(bodyBlock);

    // Driver Body
    expr->getBody()->accept(this);

    // Counting //
    // Jump to Counting
    getBuilder()->CreateBr(countBlock);

    // Set Insert Point to Counting
    parentFun->insert(parentFun->end(), countBlock);
    getBuilder()->SetInsertPoint(countBlock);

    // Check if counting expression found
    if (!isInfinity && !isSingleCondition)
    {
        auto countExpr = conditions[2];

        countExpr->accept(this);
    }

    // Jump back to Condition
    getBuilder()->CreateBr(conditionBlock);

    // End Block //
    parentFun->insert(parentFun->end(), endBlock);
    getBuilder()->SetInsertPoint(endBlock);

    removeBreakBlock();
    removeContinueBlock();

    // Exit from statement
    exitScope();

    return nullptr;
}
