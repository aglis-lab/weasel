#pragma once

#include <vector>
#include <iostream>
#include <optional>

#include <weasel/Lexer/Token.h>
#include <weasel/Basic/Error.h>
#include <weasel/Basic/Codegen.h>

namespace llvm
{
    class Type;
} // namespace llvm

namespace weasel
{
    class StructType;
    class Type;
    class GlobalVariable;

    using TypeHandle = shared_ptr<Type>;
    using StructTypeHandle = shared_ptr<StructType>;
    using GlobalVariableHandle = shared_ptr<GlobalVariable>;

    enum class TypeID
    {
        // Primitive Types
        VoidType,
        FloatType,
        DoubleType,
        IntegerType,
        AnyType,

        // Derived Types
        PointerType,   // *type     -> all time lifetime
        ArrayType,     // []type    -> fixed array -> scope lifetime; dynamic array -> all time lifetime
        ReferenceType, // &type     -> scope lifetime

        // User Type
        FunctionType,
        StructType,

        // Unknown Type
        UnknownType // @notype      -> need to check it's real type
    };

    // Data Type
    class Type
    {
        CODEGEN_TYPE

    public:
        // Create Type From Token
        static TypeHandle create(Token token);

        Type(Token token, TypeID typeId = TypeID::UnknownType) : _typeId(typeId), _token(token), _width(0), _isSigned(true) {}

        Type(TypeID typeId, uint width = 0, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign) {}

        Type(TypeID typeId, TypeHandle containedType, unsigned width = 0, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign)
        {
            _innerType = containedType;
        }

        TypeID getTypeID() const { return _typeId; }
        int getTypeWidth();

        int getTypeWidthByte() { return getTypeWidth() / 8; }

        bool isSigned() const { return _isSigned; }

        bool isBoolType() const { return isIntegerType() && _width == 1; }
        bool isFloatType() const { return _typeId == TypeID::FloatType; }
        bool isDoubleType() const { return _typeId == TypeID::DoubleType; }
        bool isIntegerType() const { return _typeId == TypeID::IntegerType; }
        bool isPrimitiveType() const
        {
            return isBoolType() ||
                   isFloatType() ||
                   isDoubleType() ||
                   isIntegerType() ||
                   isVoidType();
        }

        bool isUnknownType() const { return _typeId == TypeID::UnknownType; }
        bool isReferenceType() const { return _typeId == TypeID::ReferenceType; }
        bool isPointerType() const { return _typeId == TypeID::PointerType; }
        bool isArrayType() const { return _typeId == TypeID::ArrayType; }
        bool isVoidType() const { return _typeId == TypeID::VoidType; }
        bool isStructType() const { return _typeId == TypeID::StructType; }
        bool isValidType() const { return _width != -1; }
        bool isDerivedType() const
        {
            return isPointerType() ||
                   isArrayType() ||
                   isReferenceType() ||
                   isStructType() ||
                   isFunctionType();
        }
        bool isFunctionType() const { return _typeId == TypeID::FunctionType; }

        // Check possible struct type
        bool isPossibleStructType();

        unsigned getContainedWidth() const { return _innerType->getTypeWidth(); }
        void setContainedType(TypeHandle newType) { _innerType = newType; }
        TypeHandle getContainedType() { return _innerType; }

        // Generator
        static TypeHandle getVoidType() { return make_shared<Type>(TypeID::VoidType, 0, false); }
        static TypeHandle getAnyType(Token token) { return make_shared<Type>(token, TypeID::AnyType); }
        static TypeHandle getBoolType() { return getIntegerType(1, false); }
        static TypeHandle getIntegerType(unsigned width = 32, bool isSign = true) { return make_shared<Type>(TypeID::IntegerType, width, isSign); }
        static TypeHandle getFloatType() { return make_shared<Type>(TypeID::FloatType, 32); }
        static TypeHandle getDoubleType() { return make_shared<Type>(TypeID::DoubleType, 64); }
        static TypeHandle getArrayType(TypeHandle containedType, unsigned width) { return make_shared<Type>(TypeID::ArrayType, move(containedType), width); }
        static TypeHandle getPointerType(TypeHandle containedType) { return make_shared<Type>(TypeID::PointerType, move(containedType)); }
        static TypeHandle getReferenceType(TypeHandle containedType) { return make_shared<Type>(TypeID::ReferenceType, move(containedType)); }
        // static TypeHandle getReferenceType() { return make_shared<Type>(TypeID::ReferenceType); }
        static TypeHandle getStructType() { return make_shared<Type>(TypeID::StructType, -1); }
        static TypeHandle getUnknownType(Token token) { return make_shared<Type>(token); }

        // Check Type
        bool isEqual(TypeHandle type);

        string getTypeName();
        string getManglingName();
        int getTypeIdToInt() const { return enumToInt(_typeId); }

        void setToken(Token token) { _token = token; }
        Token getToken() const { return _token; }

        optional<Error> getError() const { return _error; }
        void setError(Error error) { _error = error; }
        bool isError() const { return _error.has_value(); }

    protected:
        bool _isSigned = true;
        int _width = 32; // width in bit

        TypeID _typeId = TypeID::VoidType;
        TypeHandle _innerType;
        Token _token = Token::create();
        optional<Error> _error;
    };

    class StructTypeField
    {
    private:
        Token _token;
        string _identifier;
        TypeHandle _type;

    public:
        StructTypeField(Token token, string identifier, TypeHandle type) : _token(token), _identifier(identifier), _type(type) {}
        ~StructTypeField() = default;

        string getIdentifier() const { return _identifier; }
        TypeHandle getType() const { return _type; }
        Token getToken() const { return _token; }

        void setType(TypeHandle type) { _type = type; }
        void setIdentifier(string identifier) { _identifier = identifier; }
    };

    // Struct Value
    class StructType : public Type
    {
        CODEGEN_TYPE

    private:
        vector<StructTypeField> _fields;
        string _identifier;

    public:
        StructType() : Type(TypeID::StructType, 0, false) {}
        StructType(string structName) : Type(TypeID::StructType, 0, false), _identifier(structName) {}

        string getIdentifier() const { return _identifier; }
        void setIdentifier(string identifier) { _identifier = identifier; }

        tuple<int, optional<StructTypeField>> findTypeName(const string &typeName);

        vector<StructTypeField> &getFields() { return _fields; }
        void addField(const StructTypeField &field)
        {
            _fields.push_back(field);
        }
        bool isPreferConstant() const
        {
            return !_innerType->isArrayType();
        }
    };

    // Function Type
    class FunctionType : public Type
    {
        CODEGEN_TYPE

    private:
        vector<TypeHandle> _arguments;
        TypeHandle _returnType;
        bool _isVararg = false;

    public:
        FunctionType() : Type(TypeID::FunctionType, 0, false) {}

        vector<TypeHandle> &getArguments() { return _arguments; }

        void setReturnType(TypeHandle type) { _returnType = type; }
        TypeHandle getReturnType() { return _returnType; }

        void setIsVararg(bool isVararg) { _isVararg = isVararg; }
        bool isVararg() const { return _isVararg; }
    };
} // namespace weasel
