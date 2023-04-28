#include <weasel/IR/Codegen.h>

llvm::MDNode *weasel::WeaselCodegen::getTBAARoot() const
{
    return getMDBuilder()->createTBAARoot("Simple C / C++ TBAA");
}

llvm::MDNode *weasel::WeaselCodegen::getTBAAChar() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("omnipotent char", getTBAARoot(), 0);
}

llvm::MDNode *weasel::WeaselCodegen::getTBAAShort() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("short", getTBAAChar(), 0);
}

llvm::MDNode *weasel::WeaselCodegen::getTBAAInt() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("int", getTBAAChar(), 0);
}

llvm::MDNode *weasel::WeaselCodegen::getTBAALong() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("long long", getTBAAChar(), 0);
}

llvm::MDNode *weasel::WeaselCodegen::getTBAAPointer() const
{
    return getMDBuilder()->createTBAAScalarTypeNode("any pointer", getTBAAChar(), 0);
}

llvm::MDNode *weasel::WeaselCodegen::getTBAA(llvm::Type *type) const
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
