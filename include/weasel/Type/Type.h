#pragma once

namespace llvm
{
    class Type;
} // namespace llvm

namespace weasel
{
    class Context;
    class Token;

    enum class TypeID
    {
        // Primitive Types
        FloatType,
        DoubleType,

        VoidType,
        IntegerType,
        PointerType,
        ArrayType,
        FunctionType,
        StructType,
    };

    // Data Type
    class Type
    {
    protected:
        bool _isSigned;
        unsigned _width = 32; // width in bit
        TypeID _typeId;
        Type *_containedType;

        Type(TypeID typeId, Type *containedType, unsigned width = 0, bool isSign = true) : _typeId(typeId), _containedType(containedType), _width(width), _isSigned(isSign) {}
        Type(TypeID typeId, unsigned width, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign) {}

    public:
        // Create Type From Token
        static Type *create(const Token &token);

    public:
        inline TypeID
        getTypeID() const
        {
            return _typeId;
        }
        inline unsigned getTypeWidth() const { return _width; }
        inline bool isSigned() const { return _isSigned; }

        inline bool isBooleanType() const { return isIntegerType() && _width == 1; }
        inline bool isFloatType() const { return _typeId == TypeID::FloatType; }
        inline bool isDoubleType() const { return _typeId == TypeID::DoubleType; }

        inline bool isIntegerType() const { return _typeId == TypeID::IntegerType; }
        inline bool isPointerType() const { return _typeId == TypeID::PointerType; }
        inline bool isArrayType() const { return _typeId == TypeID::ArrayType; }
        inline bool isVoidType() const { return _typeId == TypeID::VoidType; }

        inline unsigned getContainedWidth() const { return _containedType->getTypeWidth(); }
        inline Type *getContainedType() const { return _containedType; }

        // Generator
        static Type *getVoidType() { return new Type(TypeID::VoidType, nullptr); }
        static Type *getIntegerType(unsigned width = 32, bool isSign = true) { return new Type(TypeID::IntegerType, width, isSign); }
        static Type *getFloatType() { return new Type(TypeID::FloatType, 32); }
        static Type *getDoubleType() { return new Type(TypeID::DoubleType, 64); }
        static Type *getPointerType(Type *containedType) { return new Type(TypeID::PointerType, containedType); }
        static Type *getArrayType(Type *containedType, unsigned width) { return new Type(TypeID::ArrayType, containedType, width); }

        // Check Type
        bool isEqual(Type *type);

    public:
        llvm::Type *codegen(Context *context);
    };
} // namespace weasel
