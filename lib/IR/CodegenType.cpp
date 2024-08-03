#include <iostream>

#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>

#include "weasel/IR/Codegen.h"
#include <functional>

llvm::Type *Codegen::codegen(FunctionType *expr)
{
    // auto isVararg = expr->
    // auto retType = expr->getReturnType()->accept(this);
    // auto args = vector<llvm::Type *>();
    // for (auto &item : expr->getArguments())
    // {
    //     args.push_back(item->accept(this));
    // }

    // return llvm::FunctionType::get(retType, args, false);

    return llvm::PointerType::get(*getContext(), 0);
}

// Weasel User Type System to llvm Type System
llvm::Type *Codegen::codegen(StructType *structExpr)
{
    auto types = structExpr->getFields();
    auto typesVal = std::vector<llvm::Type *>();
    auto identifier = structExpr->getIdentifier();

    if (auto structExist = findStructType(identifier); structExist != nullptr)
    {
        return structExist;
    }

    auto structType = llvm::StructType::create(*getContext(), "struct." + identifier);
    addStructType(identifier, structType);

    for (auto item : types)
    {
        // TODO: Create more proper check for circular type
        if (item.getType()->isStructType())
        {
            if (auto itemStructType = dynamic_pointer_cast<StructType>(item.getType()); itemStructType)
            {
                assert(itemStructType->getIdentifier() != identifier && "Cannot create circular struct");
            }
        }

        typesVal.push_back(item.getType()->accept(this));
    }

    structType->setBody(typesVal);
    return structType;
}

// Weasel Type System to llvm Type System
llvm::Type *Codegen::codegen(Type *type)
{
    if (type->isVoidType())
    {
        return getBuilder()->getVoidTy();
    }

    if (type->isIntegerType())
    {
        return getBuilder()->getIntNTy(type->getTypeWidth());
    }

    if (type->isFloatType())
    {
        return getBuilder()->getFloatTy();
    }

    if (type->isDoubleType())
    {
        return getBuilder()->getDoubleTy();
    }

    if (type->isArrayType())
    {
        auto containedType = type->getContainedType();
        assert(containedType != nullptr);

        auto containedTypeV = containedType->accept(this);
        assert(containedTypeV != nullptr);

        if (type->getTypeWidth() == -1)
        {
            return llvm::PointerType::get(containedTypeV, 0);
        }

        return llvm::ArrayType::get(containedTypeV, type->getTypeWidth());
    }

    if (type->isPointerType() || type->isReferenceType())
    {
        auto containedType = type->getContainedType();
        auto containedTypeV = containedType->accept(this);

        return llvm::PointerType::get(containedTypeV, 0);
    }

    return nullptr;
}
