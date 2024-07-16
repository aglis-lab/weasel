#pragma once

#include <vector>
#include <iostream>

#include <weasel/Lexer/Token.h>

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
            _containedTypes.push_back(move(containedType));
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
        std::vector<TypeHandle> getContainedTypes() const { return _containedTypes; }
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

        std::string getTypeName();
        std::string getManglingName();

    protected:
        bool _isSpread = false;
        bool _isSigned = true;
        int _width = 32; // width in bit

        TypeID _typeId = TypeID::VoidType;
        Token _token = Token::create();
        std::vector<TypeHandle> _containedTypes;
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
        int findTypeName(const std::string &typeName);

        void addField(const std::string &fieldName, TypeHandle type);
        bool isPreferConstant();

    public:
        llvm::Type *codegen(WeaselCodegen *codegen) override;
    };

    class ArgumentType
    {
    public:
        ArgumentType(std::string argumentName, Type *type) : _argumentName(argumentName), _type(type) {}

        ArgumentType() {}

        static ArgumentTypeHandle create() { return make_shared<ArgumentType>(); }

        std::string getArgumentName() const { return _argumentName; }
        TypeHandle getType() { return _type; }

        void setArgumentName(string argumentName) { _argumentName = argumentName; }
        void setType(TypeHandle type) { _type = type; }

    private:
        std::string _argumentName;
        TypeHandle _type;
    };
} // namespace weasel
