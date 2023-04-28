#pragma once

#include <vector>
#include <iostream>
#include "weasel/Lexer/Token.h"

namespace llvm
{
    class Type;
} // namespace llvm

namespace weasel
{
    class WeaselCodegen;
    class StructType;

    enum class TypeID
    {
        // Primitive Types
        FloatType,
        DoubleType,
        VoidType,
        IntegerType,

        // Derived Types
        PointerType,
        ArrayType,

        // User Type
        FunctionType,
        StructType,
    };

    // Data Type
    class Type
    {
    public:
        // Create Type From Token
        static Type *create(Token token);

    public:
        TypeID getTypeID() const { return _typeId; }
        int getTypeWidth();

        int getTypeWidthByte() { return getTypeWidth() / 8; }

        bool isSigned() const { return _isSigned; }
        bool isSpread() const { return _isSpread; }
        void setSpread(bool val) { _isSpread = val; }

        bool isBoolType() const { return isIntegerType() && _width == 1; }
        bool isFloatType() const { return _typeId == TypeID::FloatType; }
        bool isDoubleType() const { return _typeId == TypeID::DoubleType; }
        bool isIntegerType() const { return _typeId == TypeID::IntegerType; }
        bool isPrimitiveType() const
        {
            return isBoolType() ||
                   isFloatType() ||
                   isDoubleType() ||
                   isIntegerType();
        }

        bool isPointerType() const { return _typeId == TypeID::PointerType; }
        bool isArrayType() const { return _typeId == TypeID::ArrayType; }
        bool isVoidType() const { return _typeId == TypeID::VoidType; }
        bool isStructType() const { return _typeId == TypeID::StructType; }
        bool isDerivedType() const
        {
            return isPointerType() ||
                   isArrayType() ||
                   isStructType();
        }

        unsigned getContainedWidth() const { return _containedTypes[0]->getTypeWidth(); }
        Type *getContainedType() const { return _containedTypes[0]; }
        unsigned getContainedNums() const { return _containedTypes.size(); }
        std::vector<Type *> getContainedTypes() const { return _containedTypes; }
        void addContainedType(Type *containedType) { _containedTypes.push_back(containedType); }
        void replaceContainedTypes(const std::vector<Type *> &containedTypes)
        {
            for (auto item : _containedTypes)
            {
                delete item;
                item = nullptr;
            }

            _containedTypes.clear();

            _containedTypes = containedTypes;
        }

        // Generator
        static Type *getVoidType() { return new Type(TypeID::VoidType, 0, false); }
        static Type *getBoolType() { return getIntegerType(1, false); }
        static Type *getIntegerType(unsigned width = 32, bool isSign = true) { return new Type(TypeID::IntegerType, width, isSign); }
        static Type *getFloatType() { return new Type(TypeID::FloatType, 32); }
        static Type *getDoubleType() { return new Type(TypeID::DoubleType, 64); }
        static Type *getPointerType(Type *containedType) { return new Type(TypeID::PointerType, containedType); }
        static Type *getArrayType(Type *containedType, unsigned width) { return new Type(TypeID::ArrayType, containedType, width); }

        // Check Type
        bool isEqual(Type *type);

        std::string getTypeName();

    public:
        virtual ~Type();
        virtual llvm::Type *codegen(WeaselCodegen *codegen);

    protected:
        bool _isSpread = false;
        bool _isSigned = true;
        int _width = 32; // width in bit
        TypeID _typeId = TypeID::VoidType;
        std::vector<Type *> _containedTypes;

        Type(TypeID typeId, Type *containedType, unsigned width = 0, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign)
        {
            _containedTypes.push_back(containedType);
        }
        Type(TypeID typeId, unsigned width, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign) {}
    };

    // Struct Value
    class StructType : public Type
    {
    private:
        std::vector<std::string> _typeNames;
        std::string _identifier;

        StructType(std::string structName) : Type(TypeID::StructType, 0, false) { setIdentifier(structName); }

    public:
        static StructType *get(const std::string &structName) { return new StructType(structName); }

    public:
        std::string getIdentifier() const { return _identifier; }
        void setIdentifier(std::string identifier) { _identifier = identifier; }

        std::vector<std::string> getTypeNames() const { return _typeNames; }
        int findTypeName(const std::string &typeName)
        {
            auto exist = std::find(_typeNames.begin(), _typeNames.end(), typeName);
            if (exist == _typeNames.end())
            {
                return -1;
            }

            return exist - _typeNames.begin();
        }
        void addField(const std::string &fieldName, Type *type)
        {
            _typeNames.push_back(fieldName);
            addContainedType(type);
        }

        bool isPreferConstant()
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

    public:
        llvm::Type *codegen(WeaselCodegen *codegen) override;
    };

    class ArgumentType
    {
    public:
        std::string getArgumentName() const { return _argumentName; }
        Type *getType() const { return _type; }

    public:
        static ArgumentType *create(std::string argumentName, Type *type)
        {
            return new ArgumentType(argumentName, type);
        }

    protected:
        ArgumentType(std::string argumentName, Type *type) : _argumentName(argumentName), _type(type) {}

    private:
        std::string _argumentName;
        Type *_type;
    };
} // namespace weasel
