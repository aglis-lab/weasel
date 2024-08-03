#include "weasel/IR/Codegen.h"

llvm::MDNode *Codegen::getTBAARoot()
{
    return getMDBuilder()->createTBAARoot("Simple C / C++ TBAA");
}

llvm::MDNode *Codegen::getTBAAChar()
{
    return getMDBuilder()->createTBAAScalarTypeNode("omnipotent char", getTBAARoot(), 0);
}

llvm::MDNode *Codegen::getTBAAShort()
{
    return getMDBuilder()->createTBAAScalarTypeNode("short", getTBAAChar(), 0);
}

llvm::MDNode *Codegen::getTBAAInt()
{
    return getMDBuilder()->createTBAAScalarTypeNode("int", getTBAAChar(), 0);
}

llvm::MDNode *Codegen::getTBAALong()
{
    return getMDBuilder()->createTBAAScalarTypeNode("long long", getTBAAChar(), 0);
}

llvm::MDNode *Codegen::getTBAAPointer()
{
    return getMDBuilder()->createTBAAScalarTypeNode("any pointer", getTBAAChar(), 0);
}

llvm::MDNode *Codegen::getTBAA(llvm::Type *type)
{
    llvm::MDNode *node;
    auto width = type->getIntegerBitWidth();

    if (type->isPointerTy())
    {
        node = getTBAAPointer();
    }
    else if (width == 8u)
    {
        node = getTBAAChar();
    }
    else if (width == 16u)
    {
        node = getTBAAShort();
    }
    else if (width == 32u)
    {
        node = getTBAAInt();
    }
    else if (width == 64u)
    {
        node = getTBAALong();
    }
    else
    {
        assert(false && "You get another bit width");
    }

    return node;
}
