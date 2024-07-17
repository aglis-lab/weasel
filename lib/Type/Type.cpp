#include <iostream>

#include <fmt/core.h>

#include "weasel/IR/Codegen.h"
#include "weasel/Type/Type.h"

using namespace weasel;

std::string weasel::Type::getManglingName()
{
    switch (getTypeID())
    {
    case TypeID::ArrayType:
        return "A" + getContainedType()->getManglingName();
    case TypeID::FloatType:
        return "f";
    case TypeID::DoubleType:
        return "d";
    case TypeID::VoidType:
        return "v";
    case TypeID::PointerType:
        return "T" + getContainedType()->getManglingName();
    case TypeID::ReferenceType:
        return "C" + getContainedType()->getManglingName();
    case TypeID::StructType:
        return dynamic_cast<StructType *>(this)->getIdentifier();
    case TypeID::IntegerType:
    {
        if (_width == 1)
        {
            return "b";
        }
        else if (_width == 8)
        {
            if (_isSigned)
            {
                return "sb";
            }
            else
            {
                return "ub";
            }
        }
        else if (_width == 16)
        {
            if (_isSigned)
            {
                return "s";
            }
            else
            {
                return "us";
            }
        }
        else if (_width == 32)
        {
            if (_isSigned)
            {
                return "i";
            }
            else
            {
                return "ui";
            }
        }
        else if (_width == 64)
        {
            if (_isSigned)
            {
                return "l";
            }
            else
            {
                return "ul";
            }
        }
        return "uncover integer mangling type";
    }
    default:
        return "uncover mangling type";
    }
}

bool weasel::Type::isPossibleStructType()
{
    if (isStructType())
    {
        return true;
    }

    if (isPointerType())
    {
        return getContainedType()->isStructType();
    }

    if (isReferenceType())
    {
        return getContainedType()->isStructType();
    }

    return false;
}

int StructType::findTypeName(const string &typeName)
{
    for (size_t i = 0; i < _fields.size(); i++)
    {
        if (_fields[i].getIdentifier() == typeName)
        {
            return i;
        }
    }

    return -1;
}

int weasel::Type::getTypeWidth()
{
    if (isStructType())
    {
        auto val = 0;

        for (size_t i = 0; i < getContainedTypes().size(); i++)
        {
            auto item = getContainedTypes()[i].get();
            val += item->getTypeWidth();
        }

        return val;
    }

    if (isPointerType())
    {
        return 64;
    }

    return _width;
}

weasel::Type::~Type()
{
    getContainedTypes().clear();
}

llvm::Type *weasel::StructType::codegen(weasel::WeaselCodegen *context)
{
    return context->codegen(this);
}

llvm::Type *weasel::Type::codegen(weasel::WeaselCodegen *context)
{
    return context->codegen(this);
}

bool Type::isEqual(TypeHandle type)
{
    if (!type)
    {
        return false;
    }

    if (auto val = this->getTypeID() == type->getTypeID(); !val)
    {
        return false;
    }

    if (isPrimitiveType())
    {
        return true;
    }

    if (isDerivedType())
    {
        return getContainedType()->isEqual(type->getContainedType());
    }

    // User Type Check
    // There is just one or single instance for user type
    // Used for struct type
    return this == type.get();
}

TypeHandle Type::create(Token token)
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
        return Type::getUnknownType();
    }
}

std::string weasel::Type::getTypeName()
{
    switch (getTypeID())
    {
    case TypeID::IntegerType:
    {
        auto prefix = "";
        std::string type;
        if (!_isSigned)
        {
            prefix = "u";
        }

        if (_width == 1)
        {
            type = "bool";
            prefix = "";
        }
        else if (_width == 8)
        {
            type = "byte";

            if (_isSigned)
            {
                prefix = "s";
            }
            else
            {
                prefix = "";
            }
        }
        else if (_width == 16)
        {
            type = "short";
        }
        else if (_width == 32)
        {
            type = "int";
        }
        else if (_width == 64)
        {
            type = "long";
        }

        return fmt::format("@{}{}", prefix, type);
    }
    case TypeID::DoubleType:
    {
        return "@double";
    }
    case TypeID::FloatType:
    {
        return "@float";
    }
    case TypeID::VoidType:
    {
        return "@void";
    }
    case TypeID::PointerType:
    {
        return fmt::format("{}{}", '*', this->getContainedType()->getTypeName());
    }
    case TypeID::ReferenceType:
    {
        return fmt::format("{}{}", '&', this->getContainedType()->getTypeName());
    }
    case TypeID::ArrayType:
    {
        return fmt::format("[]{}", this->getContainedType()->getTypeName());
    }
    case TypeID::StructType:
    {
        auto val = dynamic_cast<StructType *>(this);
        return fmt::format("@{}", val->getIdentifier());
    }
    default:
    {
        return "@no-type";
    }
    }
}
