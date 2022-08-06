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
        auto containedType = type->getContainedType()->codegen(this);
        return llvm::ArrayType::get(containedType, type->getTypeWidth());
    }

    if (type->isPointerType())
    {
        auto containedType = type->getContainedType()->codegen(this);
        return llvm::PointerType::get(containedType, type->getTypeWidth());
    }

    return nullptr;
}

// Compare Type Helpter
weasel::CompareType weasel::Context::compareType(llvm::Type *lhsType, llvm::Type *rhsType)
{
    if (lhsType->getTypeID() != rhsType->getTypeID())
    {
        return CompareType::Different;
    }

    if (lhsType->isIntegerTy())
    {
        if (lhsType->getIntegerBitWidth() != rhsType->getIntegerBitWidth())
        {
            return CompareType::Casting;
        }
    }

    return CompareType::Equal;
}

// Cast Integer Type Helper
llvm::Value *weasel::Context::castIntegerType(llvm::Value *value, llvm::Type *castTy)
{
    if (value->getType()->getIntegerBitWidth() < castTy->getIntegerBitWidth())
    {
        return getBuilder()->CreateCast(llvm::Instruction::CastOps::SExt, value, castTy);
    }
    else
    {
        return getBuilder()->CreateCast(llvm::Instruction::CastOps::Trunc, value, castTy);
    }
}

// Cast Integer Type Helper
llvm::Value *weasel::Context::castIntegerType(llvm::Value *lhs, llvm::Value *rhs) const
{
    if (lhs->getType()->getIntegerBitWidth() > rhs->getType()->getIntegerBitWidth())
    {
        return getBuilder()->CreateCast(llvm::Instruction::CastOps::SExt, rhs, lhs->getType());
    }
    else
    {
        return getBuilder()->CreateCast(llvm::Instruction::CastOps::SExt, lhs, rhs->getType());
    }
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
