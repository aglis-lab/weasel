#include <iostream>

#include <fmt/core.h>

#include <weasel/IR/Codegen.h>
#include <weasel/Type/Type.h>

int weasel::StructType::findTypeName(const std::string &typeName)
{
    auto exist = std::find(_typeNames.begin(), _typeNames.end(), typeName);
    if (exist == _typeNames.end())
    {
        return -1;
    }

    return exist - _typeNames.begin();
}

void weasel::StructType::addField(const std::string &fieldName, Type *type)
{
    _typeNames.push_back(fieldName);

    addContainedType(type);
}

bool weasel::StructType::isPreferConstant()
{
    for (auto &item : getContainedTypes())
    {
        if (item->isArrayType())
        {
            return false;
        }
    }

    return true;
}

void weasel::Type::replaceContainedTypes(const std::vector<Type *> &containedTypes)
{
    for (auto item : _containedTypes)
    {
        delete item;
        item = nullptr;
    }

    _containedTypes.clear();

    _containedTypes = containedTypes;
}

int weasel::Type::getTypeWidth()
{
    if (isStructType())
    {
        auto val = 0;
        for (auto item : getContainedTypes())
        {
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

weasel::Type *weasel::Type::create(Token token)
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

std::string weasel::Type::getTypeName()
{
    switch (getTypeID())
    {
    case TypeID::IntegerType:
    {
        char prefix = '\0';
        std::string type;
        if (!_isSigned)
        {
            prefix = 'u';
        }

        if (_width == 1)
        {
            type = "bool";
            prefix = '\0';
        }

        if (_width == 8)
        {
            type = "byte";

            if (_isSigned)
            {
                prefix = 's';
            }
            else
            {
                prefix = '\0';
            }
        }

        if (_width == 16)
        {
            type = "short";
        }

        if (_width == 32)
        {
            type = "int";
        }

        if (_width == 64)
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
    case TypeID::ArrayType:
    {
        return fmt::format("[]{}", this->getContainedType()->getTypeName());
    }
    default:
    {
        return "@no-type";
    }
    }
}
