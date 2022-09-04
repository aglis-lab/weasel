#include <iostream>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"
#include "weasel/Config/Config.h"

weasel::Context::Context(llvm::LLVMContext *context, const std::string &moduleName)
{
    _context = context;
    _module = new llvm::Module(moduleName, *_context);
    _mdBuilder = new llvm::MDBuilder(*_context);
    _builder = new llvm::IRBuilder<>(*_context);
}

llvm::Value *weasel::Context::codegen(weasel::Function *funAST)
{
    _currentFunction = funAST;

    auto funName = funAST->getIdentifier();
    auto funType = funAST->getType();
    auto isVararg = funType->isSpread();
    auto funArgs = funType->getContainedTypes();
    auto argsLength = (int)funArgs.size() - (isVararg ? 1 : 0);
    auto args = std::vector<llvm::Type *>(argsLength);
    for (int index = 0; index < argsLength; index++)
    {
        args[index] = funArgs[index]->codegen(this);
    }

    auto linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
    auto funTyLLVM = llvm::FunctionType::get(funType->codegen(this), args, isVararg);
    auto funLLVM = llvm::Function::Create(funTyLLVM, linkage, funName, getModule());
    if (funAST->isInline())
    {
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::InlineHint);
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::AlwaysInline);
    }

    funLLVM->setDSOLocal(true);

    if (funAST->isDefine())
    {
        auto entry = llvm::BasicBlock::Create(*getContext(), "entry", funLLVM);
        getBuilder()->SetInsertPoint(entry);

        // Enter to new scope
        enterScope();

        auto idx = 0;
        for (auto &item : funLLVM->args())
        {
            auto argExpr = funArgs[idx++];
            auto argName = argExpr->getIdentifier();

            item.setName(argName);

            // Store Param to new variable
            auto alloc = getBuilder()->CreateAlloca(item.getType());

            // Store
            getBuilder()->CreateStore(&item, alloc);

            addAttribute(ContextAttribute::get(argName, alloc, AttributeKind::Parameter));
        }

        // Create Block
        funAST->getBody()->codegen(this);
        if (funLLVM->getReturnType()->isVoidTy() && llvm::dyn_cast<llvm::ReturnInst>(&getBuilder()->GetInsertBlock()->back()) == nullptr)
        {
            getBuilder()->CreateRetVoid();
        }

        // Exit fromscope
        exitScope();
    }

    return funLLVM;
}

llvm::Value *weasel::Context::codegen(CallExpression *expr)
{
    auto identifier = expr->getIdentifier();
    auto args = expr->getArguments();
    auto fun = getModule()->getFunction(identifier);

    std::vector<llvm::Value *> argsV;
    for (size_t i = 0; i < args.size(); i++)
    {
        argsV.push_back(args[i]->codegen(this));
        if (!argsV.back())
        {
            return ErrorTable::addError(expr->getToken(), "Expected argument list index " + std::to_string(i));
        }
    }

    auto call = getBuilder()->CreateCall(fun, argsV);

    call->setCallingConv(llvm::CallingConv::C);

    return call;
}

llvm::Value *weasel::Context::codegen(BreakExpression *expr)
{
    if (!isBreakBlockExist())
    {
        return ErrorTable::addError(expr->getToken(), "No looping found");
    }

    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateBr(getBreakBlock());
    }

    auto condExpr = expr->getValue();
    auto condType = condExpr->getType();
    if (!condType->isBoolType())
    {
        return ErrorTable::addError(expr->getToken(), "Break Condition should be boolean");
    }

    auto newBlock = llvm::BasicBlock::Create(*getContext(), "", getBuilder()->GetInsertBlock()->getParent());
    auto breakBlock = getBreakBlock();
    auto condVal = condExpr->codegen(this);
    auto brIns = getBuilder()->CreateCondBr(condVal, breakBlock, newBlock);

    // Create New Insert Point
    getBuilder()->SetInsertPoint(newBlock);

    return brIns;
}

llvm::Value *weasel::Context::codegen(ContinueExpression *expr)
{
    if (!isContinueBlockExist())
    {
        return ErrorTable::addError(expr->getToken(), "No looping found");
    }

    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateBr(getContinueBlock());
    }

    auto condExpr = expr->getValue();
    auto condType = condExpr->getType();
    if (!condType->isBoolType())
    {
        return ErrorTable::addError(expr->getToken(), "Continue Condition should be boolean");
    }

    auto newBlock = llvm::BasicBlock::Create(*getContext(), "", getBuilder()->GetInsertBlock()->getParent());
    auto continueBlock = getContinueBlock();
    auto condVal = condExpr->codegen(this);
    auto brIns = getBuilder()->CreateCondBr(condVal, continueBlock, newBlock);

    // // Create New Insert Point
    getBuilder()->SetInsertPoint(newBlock);

    return brIns;
}

llvm::Value *weasel::Context::codegen(ReturnExpression *expr)
{
    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateRetVoid();
    }

    // Get Last Function from symbol table
    auto fun = getBuilder()->GetInsertBlock()->getParent();
    auto returnTyV = fun->getReturnType();

    auto val = expr->getValue()->codegen(this);
    assert(val && "Expression value cannot be null value");

    if (returnTyV->getTypeID() != val->getType()->getTypeID())
    {
        return ErrorTable::addError(expr->getToken(), "Return Type with value type is different");
    }

    if (returnTyV->isIntegerTy())
    {
        if (expr->getType()->isSigned())
        {
            val = getBuilder()->CreateSExtOrTrunc(val, returnTyV);
        }
        else
        {
            val = getBuilder()->CreateZExtOrTrunc(val, returnTyV);
        }
    }

    return getBuilder()->CreateRet(val);
}

llvm::Value *weasel::Context::codegen(VariableExpression *expr)
{
    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto type = expr->getType();
    auto attr = findAttribute(varName);
    if (attr.isEmpty())
    {
        return ErrorTable::addError(expr->getToken(), "Variable " + varName + " Not declared");
    }

    auto alloc = attr.getValue();
    if (llvm::dyn_cast<llvm::Argument>(alloc))
    {
        return alloc;
    }

    if (type->isStructType())
    {
        return getBuilder()->CreateBitCast(alloc, getBuilder()->getInt8PtrTy());
    }

    if (expr->isLHS())
    {
        return alloc;
    }

    auto typeVal = type->codegen(this);
    return getBuilder()->CreateLoad(typeVal, alloc);
}

// TODO: String as array of byte
llvm::Value *weasel::Context::codegen(ArrayExpression *expr)
{
    // Get Index
    auto indexExpr = expr->getIndex();
    assert(indexExpr);

    auto indexV = indexExpr->codegen(this);
    assert(indexV);

    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto attr = findAttribute(varName);
    assert(!attr.isEmpty());

    auto alloc = attr.getValue();
    assert(alloc);

    // Casting to integer 64
    indexV = getBuilder()->CreateSExtOrTrunc(indexV, getBuilder()->getInt64Ty());

    std::vector<llvm::Value *> idxList = {
        getBuilder()->getInt64(0),
        indexV,
    };

    auto typeV = alloc->getType()->getPointerElementType();
    auto elemIndex = getBuilder()->CreateInBoundsGEP(typeV, alloc, idxList);

    if (expr->isLHS())
    {
        return elemIndex;
    }

    return getBuilder()->CreateLoad(typeV, elemIndex);
}

llvm::Value *weasel::Context::codegen(FieldExpression *expr)
{
    auto parent = dynamic_cast<VariableExpression *>(expr->getParent());
    auto type = dynamic_cast<StructType *>(parent->getType());
    assert(type != nullptr);

    auto typeV = type->codegen(this);
    auto attr = findAttribute(parent->getIdentifier());
    auto field = expr->getField();
    auto idx = type->findTypeName(field);

    auto range = getBuilder()->getInt32(0);
    auto idxVal = getBuilder()->getInt32(idx);
    auto inbound = getBuilder()->CreateInBoundsGEP(typeV, attr.getValue(), {range, idxVal});

    return getBuilder()->CreateLoad(inbound->getType()->getPointerElementType(), inbound);
}

llvm::Value *weasel::Context::codegen(StructExpression *expr)
{
    if (expr->getFields().empty())
    {
        return getBuilder()->getInt8(0);
    }

    auto type = dynamic_cast<StructType *>(expr->getType());
    auto isConstant = expr->getIsPreferConstant() && type->isPreferConstant();
    auto fields = expr->getFields();
    if (isConstant)
    {
        for (auto item : fields)
        {
            if (!dynamic_cast<LiteralExpression *>(item->getExpression()))
            {
                isConstant = false;
                break;
            }
        }
    }

    auto typeFields = type->getContainedTypes();
    auto fieldSize = (int)typeFields.size();
    if (isConstant)
    {
        std::vector<llvm::Constant *> arr(fieldSize, nullptr);
        for (auto &item : fields)
        {
            auto idx = type->findTypeName(item->getIdentifier());
            if (idx == -1)
            {
                continue;
            }

            arr[idx] = llvm::dyn_cast<llvm::Constant>(item->getExpression()->codegen(this));
        }

        for (int i = 0; i < fieldSize; i++)
        {
            if (arr[i] != nullptr)
            {
                continue;
            }

            auto typeField = typeFields[i];
            auto typeFieldV = typeField->codegen(this);
            if (typeField->isPrimitiveType())
            {
                if (typeField->isFloatType() || typeField->isDoubleType())
                {
                    arr[i] = llvm::ConstantFP::get(typeFieldV, 0);
                }
                else
                {
                    arr[i] = llvm::ConstantInt::get(typeFieldV, 0);
                }
            }
            else
            {
                arr[i] = llvm::ConstantStruct::get(llvm::dyn_cast<llvm::StructType>(typeFieldV), {});
            }
        }

        auto typeV = llvm::dyn_cast<llvm::StructType>(type->codegen(this));
        auto constantV = llvm::ConstantStruct::get(typeV, arr);
        auto val = new llvm::GlobalVariable(*getModule(), typeV, true, llvm::GlobalValue::LinkageTypes::PrivateLinkage, constantV);

        val->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

        return val;
    }

    auto typeV = type->codegen(this);
    auto alloc = getBuilder()->CreateAlloca(typeV);

    getBuilder()->CreateMemSet(alloc, getBuilder()->getInt8(0), type->getTypeWidthByte(), llvm::MaybeAlign(4));

    for (auto item : fields)
    {
        auto idx = type->findTypeName(item->getIdentifier());
        if (idx == -1)
        {
            continue;
        }

        auto idxStruct = getBuilder()->getInt32(0);
        auto idxVal = getBuilder()->getInt32(idx);
        auto inbound = getBuilder()->CreateInBoundsGEP(typeV, alloc, {idxStruct, idxVal});
        auto val = item->getExpression()->codegen(this);

        getBuilder()->CreateStore(val, inbound);
    }

    return alloc;
}
