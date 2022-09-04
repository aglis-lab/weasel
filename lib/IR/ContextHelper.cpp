#include "weasel/IR/Context.h"
#include "weasel/Config/Config.h"
#include "weasel/Symbol/Symbol.h"

std::string weasel::Context::getDefaultLabel()
{
    return std::to_string(_counter++);
}

// Weasel User Type System to llvm Type System
llvm::Type *weasel::Context::codegen(weasel::StructType *structExpr)
{
    auto types = structExpr->getContainedTypes();
    auto typesVal = std::vector<llvm::Type *>();
    auto identifier = structExpr->getIdentifier();
    auto structExist = findStructType(identifier);
    if (structExist != nullptr)
    {
        return structExist;
    }

    auto structType = llvm::StructType::create(*getContext(), identifier);
    addStructType(identifier, structType);

    for (auto item : types)
    {
        if (item->isStructType() && item->getIdentifier() == identifier)
        {
            ErrorTable::addError(Token(), "Cannot create circular struct");
            continue;
        }

        typesVal.push_back(item->codegen(this));
    }

    structType->setBody(typesVal);
    return structType;
}

// Weasel Type System to llvm Type System
llvm::Type *weasel::Context::codegen(weasel::Type *type)
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

    if (type->isPointerType())
    {
        auto containedType = type->getContainedType();
        auto containedTypeV = containedType->codegen(this);

        return llvm::PointerType::get(containedTypeV, 0);
    }

    return nullptr;
}

llvm::MDNode *weasel::Context::getTBAARoot() const
{
    return getMDBuilder()->createTBAARoot("Simple C / C++ TBAA");
}

llvm::MDNode *weasel::Context::getTBAAChar() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("omnipotent char", getTBAARoot(), 0);
}

llvm::MDNode *weasel::Context::getTBAAShort() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("short", getTBAAChar(), 0);
}

llvm::MDNode *weasel::Context::getTBAAInt() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("int", getTBAAChar(), 0);
}

llvm::MDNode *weasel::Context::getTBAALong() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("long long", getTBAAChar(), 0);
}

llvm::MDNode *weasel::Context::getTBAAPointer() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("any pointer", getTBAAChar(), 0);
}

llvm::MDNode *weasel::Context::getTBAA(llvm::Type *type) const
{
    llvm::MDNode *node;
    auto width = type->getIntegerBitWidth();

    if (type->isPointerTy())
    {
        node = getTBAAPointer();
    }
    else if (width == 8)
    {
        node = getTBAAChar();
    }
    else if (width == 16)
    {
        node = getTBAAShort();
    }
    else if (width == 32)
    {
        node = getTBAAInt();
    }
    else if (width == 64)
    {
        node = getTBAALong();
    }
    else
    {
        assert(false && "You get another bit width");
    }

    return node;
}
