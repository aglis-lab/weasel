#include <iostream>
#include "weasel/IR/Context.h"
#include "weasel/Type/Type.h"

llvm::Type *weasel::StructType::codegen(weasel::Context *context)
{
    return context->codegen(this);
}

llvm::Type *weasel::Type::codegen(weasel::Context *context)
{
    return context->codegen(this);
}

bool weasel::Type::isEqual(weasel::Type *type)
{
    if (type == nullptr)
    {
        return false;
    }

    auto val = this->getTypeID() == type->getTypeID();
    if (!val)
    {
        return false;
    }

    if (this->isPrimitiveType())
    {
        return true;
    }

    if (this->isDerivedType())
    {
        return this->getContainedType()->isEqual(type->getContainedType());
    }

    // User Type Check
    // There is just one or single instance for user type
    // Used for struct type
    return this == type;
}

weasel::Type *weasel::Type::create(const Token &token)
{
    switch (token.getTokenKind())
    {
    // BOOL //
    case TokenKind::TokenTyBool:
        return Type::getIntegerType(1);

    // INTEGER //
    case TokenKind::TokenTyByte:
        return Type::getIntegerType(8, false);

    case TokenKind::TokenTySbyte:
        return Type::getIntegerType(8);

    case TokenKind::TokenTyShort:
        return Type::getIntegerType(16);

    case TokenKind::TokenTyUshort:
        return Type::getIntegerType(16, false);

    case TokenKind::TokenTyInt:
        return Type::getIntegerType(32);

    case TokenKind::TokenTyUint:
        return Type::getIntegerType(32, false);

    case TokenKind::TokenTyLong:
        return Type::getIntegerType(64);

    case TokenKind::TokenTyUlong:
        return Type::getIntegerType(64, false);

    // FLOATING POINT //
    case TokenKind::TokenTyFloat:
        return Type::getFloatType();

    case TokenKind::TokenTyDouble:
        return Type::getDoubleType();

    // VOID //
    case TokenKind::TokenTyVoid:
        return Type::getVoidType();

    default:
        return nullptr;
    }
}
