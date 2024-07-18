#pragma once

#include <vector>
#include <iostream>

#include <weasel/Lexer/Token.h>
#include <weasel/Basic/Error.h>

namespace llvm
{
    class Type;
} // namespace llvm

namespace weasel
{
    class WeaselCodegen;
    class StructType;
    class Printer;
    class Type;
    class ArgumentType;
    class GlobalVariable;

    using TypeHandle = shared_ptr<Type>;
    using StructTypeHandle = shared_ptr<StructType>;
    using ArgumentTypeHandle = shared_ptr<ArgumentType>;
    using GlobalVariableHandle = shared_ptr<GlobalVariable>;

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
        ReferenceType,

        // User Type
        FunctionType,
        StructType,

        // Unknown Type
        UnknownType
    };

    // Data Type
    class Type
    {
    public:
        // Create Type From Token
        static TypeHandle create(Token token);

        Type() : _typeId(TypeID::UnknownType), _width(0), _isSigned(true) {}

        Type(TypeID typeId, uint width = 0, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign) {}

        Type(TypeID typeId, TypeHandle containedType, unsigned width = 0, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign)
        {
            _containedTypes.push_back(containedType);
        }

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
                   isStructType();
        }

        // Check possible struct type
        bool isPossibleStructType();

        unsigned getContainedWidth() const { return _containedTypes[0]->getTypeWidth(); }
        TypeHandle getContainedType() { return _containedTypes[0]; }
        unsigned getContainedNums() const { return _containedTypes.size(); }
        vector<TypeHandle> getContainedTypes() const { return _containedTypes; }
        void addContainedType(TypeHandle containedType) { _containedTypes.push_back(move(containedType)); }

        // Generator
        static TypeHandle getVoidType() { return make_shared<Type>(TypeID::VoidType, 0, false); }
        static TypeHandle getBoolType() { return getIntegerType(1, false); }
        static TypeHandle getIntegerType(unsigned width = 32, bool isSign = true) { return make_shared<Type>(TypeID::IntegerType, width, isSign); }
        static TypeHandle getFloatType() { return make_shared<Type>(TypeID::FloatType, 32); }
        static TypeHandle getDoubleType() { return make_shared<Type>(TypeID::DoubleType, 64); }
        static TypeHandle getArrayType(TypeHandle containedType, unsigned width) { return make_shared<Type>(TypeID::ArrayType, move(containedType), width); }
        static TypeHandle getPointerType(TypeHandle containedType) { return make_shared<Type>(TypeID::PointerType, move(containedType)); }
        static TypeHandle getReferenceType(TypeHandle containedType) { return make_shared<Type>(TypeID::ReferenceType, move(containedType)); }
        static TypeHandle getReferenceType() { return make_shared<Type>(TypeID::ReferenceType); }
        static TypeHandle getStructType() { return make_shared<Type>(TypeID::StructType, -1); }
        static TypeHandle getUnknownType() { return make_shared<Type>(TypeID::UnknownType); }

        // Check Type
        bool isEqual(TypeHandle type);

    public:
        virtual ~Type();
        virtual llvm::Type *codegen(WeaselCodegen *codegen);

        string getTypeName();
        string getManglingName();

    protected:
        bool _isSpread = false;
        bool _isSigned = true;
        int _width = 32; // width in bit

        TypeID _typeId = TypeID::VoidType;
        Token _token = Token::create();
        vector<TypeHandle> _containedTypes;
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
    };

    // Struct Value
    class StructType : public Type
    {
    private:
        vector<StructTypeField> _fields;
        string _identifier;
        optional<Error> _error;

    public:
        StructType() : Type(TypeID::StructType, 0, false) {}
        StructType(string structName) : Type(TypeID::StructType, 0, false), _identifier(structName) {}

        optional<Error> getError() const { return _error; }
        void setError(Error error) { _error = error; }

        string getIdentifier() const { return _identifier; }
        void setIdentifier(string identifier) { _identifier = identifier; }

        int findTypeName(const string &typeName);

        vector<StructTypeField> &getFields() { return _fields; }
        void addField(const StructTypeField &field)
        {
            _fields.push_back(field);
        }
        bool isPreferConstant() const
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

        void setToken(Token token) { _token = token; }
        Token getToken() const { return _token; }

        bool isError() const { return _error.has_value(); }

    public:
        llvm::Type *codegen(WeaselCodegen *codegen) override;
    };

    class ArgumentType
    {
    public:
        ArgumentType(string argumentName, Type *type) : _argumentName(argumentName), _type(type) {}

        ArgumentType() {}

        static ArgumentTypeHandle create() { return make_shared<ArgumentType>(); }

        string getArgumentName() const { return _argumentName; }
        TypeHandle getType() { return _type; }

        void setArgumentName(string argumentName) { _argumentName = argumentName; }
        void setType(TypeHandle type) { _type = type; }

    private:
        string _argumentName;
        TypeHandle _type;
    };
} // namespace weasel
