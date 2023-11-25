#include <iostream>

#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>

#include "weasel/IR/Codegen.h"
#include "weasel/Symbol/Symbol.h"

// Weasel User Type System to llvm Type System
llvm::Type *weasel::WeaselCodegen::codegen(weasel::StructType *structExpr)
{
    auto types = structExpr->getContainedTypes();
    auto typesVal = std::vector<llvm::Type *>();
    auto identifier = structExpr->getIdentifier();
    auto structExist = findStructType(identifier);
    if (structExist != nullptr)
    {
        return structExist;
    }

    auto structType = llvm::StructType::create(*getContext(), "struct." + identifier);
    addStructType(identifier, structType);

    for (auto item : types)
    {
        if (item->isStructType())
        {
            auto itemStructType = dynamic_cast<StructType *>(item);
            if (itemStructType && itemStructType->getIdentifier() == identifier)
            {
                ErrorTable::addError(Token::create(), "Cannot create circular struct");
                continue;
            }
        }

        typesVal.push_back(item->codegen(this));
    }

    structType->setBody(typesVal);
    return structType;
}

// Weasel Type System to llvm Type System
llvm::Type *weasel::WeaselCodegen::codegen(weasel::Type *type)
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

        auto containedTypeV = containedType->codegen(this);
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
        auto containedTypeV = containedType->codegen(this);

        return llvm::PointerType::get(containedTypeV, 0);
    }

    return nullptr;
}
