#pragma once

#include <vector>
#include <iostream>

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
    protected:
        bool _isSpread = false;
        bool _isSigned = true;
        unsigned _width = 32; // width in bit
        TypeID _typeId = TypeID::VoidType;
        std::vector<Type *> _containedTypes;
        std::string _identifier;

        Type(TypeID typeId, Type *containedType, unsigned width = 0, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign)
        {
            _containedTypes.push_back(containedType);
        }
        Type(TypeID typeId, unsigned width, bool isSign = true) : _typeId(typeId), _width(width), _isSigned(isSign) {}

    public:
        // Create Type From Token
        static Type *create(const Token &token);

    public:
        inline TypeID getTypeID() const { return _typeId; }
        inline unsigned getTypeWidth() const { return _width; }
        inline bool isSigned() const { return _isSigned; }
        inline bool isSpread() const { return _isSpread; }

        inline std::string getIdentifier() const { return _identifier; }
        inline void setIdentifier(std::string identifier) { _identifier = identifier; }
        inline void setSpread(bool val) { _isSpread = val; }

        inline bool isBooleanType() const { return isIntegerType() && _width == 1; }
        inline bool isFloatType() const { return _typeId == TypeID::FloatType; }
        inline bool isDoubleType() const { return _typeId == TypeID::DoubleType; }
        inline bool isIntegerType() const { return _typeId == TypeID::IntegerType; }
        inline bool isPrimitiveType() const
        {
            return isBooleanType() ||
                   isFloatType() ||
                   isDoubleType() ||
                   isIntegerType() ||
                   isVoidType();
        }

        inline bool isPointerType() const { return _typeId == TypeID::PointerType; }
        inline bool isArrayType() const { return _typeId == TypeID::ArrayType; }
        inline bool isVoidType() const { return _typeId == TypeID::VoidType; }
        inline bool isStructType() const { return _typeId == TypeID::StructType; }
        inline bool isDerivedType() const
        {
            return isPointerType() ||
                   isArrayType() ||
                   isStructType();
        }

        inline unsigned getContainedWidth() const { return _containedTypes[0]->getTypeWidth(); }
        inline Type *getContainedType() const { return _containedTypes[0]; }
        inline unsigned getContainedNums() const { return _containedTypes.size(); }
        inline std::vector<Type *> getContainedTypes() const { return _containedTypes; }
        inline void addContainedType(Type *containedType) { _containedTypes.push_back(containedType); }
        inline void replaceContainedTypes(const std::vector<Type *> &containedTypes)
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
        static Type *getIntegerType(unsigned width = 32, bool isSign = true) { return new Type(TypeID::IntegerType, width, isSign); }
        static Type *getFloatType() { return new Type(TypeID::FloatType, 32); }
        static Type *getDoubleType() { return new Type(TypeID::DoubleType, 64); }
        static Type *getPointerType(Type *containedType) { return new Type(TypeID::PointerType, containedType); }
        static Type *getArrayType(Type *containedType, unsigned width) { return new Type(TypeID::ArrayType, containedType, width); }

        // Check Type
        bool isEqual(Type *type);

        virtual ~Type()
        {
            getContainedTypes().clear();
        }

    public:
        virtual llvm::Type *codegen(Context *context);
    };

    // Struct Value
    class StructType : public Type
    {
    private:
        std::vector<std::string> _typeNames;

        StructType(std::string structName) : Type(TypeID::StructType, 0, false) { setIdentifier(structName); }

    public:
        static StructType *get(const std::string &structName) { return new StructType(structName); }

    public:
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

        inline unsigned getStructTypeWidth() const
        {
            auto val = 0;
            for (auto item : getContainedTypes())
            {
                if (item->isPointerType())
                {
                    val += 64;
                }
                else
                {
                    val += item->getTypeWidth();
                }
            }

            return val / 8;
        }

        ~StructType() {}

    public:
        llvm::Type *codegen(Context *context) override;
    };

    // class ArgumentType : public Type
    // {
    // private:
    //     std::vector<std::string> _typeNames;

    //     ArgumentType(std::string structName) : Type(TypeID::StructType, 0, false) { setIdentifier(structName); }
    // };
} // namespace weasel
